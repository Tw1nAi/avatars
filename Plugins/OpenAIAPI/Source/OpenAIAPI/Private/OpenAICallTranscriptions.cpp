// Copyright Kellan Mythen 2023. All rights Reserved.

#include "OpenAICallTranscriptions.h"

#include "Dom/JsonObject.h"
#include "Http.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "OpenAIUtils.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

UOpenAICallTranscriptions::UOpenAICallTranscriptions() {}

UOpenAICallTranscriptions::~UOpenAICallTranscriptions() {}

UOpenAICallTranscriptions* UOpenAICallTranscriptions::OpenAICallTranscriptions(FString FileName, FString Language, bool bDebug)
{
  UOpenAICallTranscriptions* BPNode = NewObject<UOpenAICallTranscriptions>();
  BPNode->FileName = FileName + ".wav";
  BPNode->bDebug = bDebug;
  BPNode->LanguageISO = Language;
  return BPNode;
}

void UOpenAICallTranscriptions::Activate()
{
  FString _apiKey;
  if (UOpenAIUtils::getUseApiKeyFromEnvironmentVars())
    _apiKey = UOpenAIUtils::GetEnvironmentVariable(TEXT("OPENAI_API_KEY"));
  else
    _apiKey = UOpenAIUtils::getApiKey();

  // checking parameters are valid
  if (_apiKey.IsEmpty())
  {
    Finished.Broadcast({}, TEXT("Api key is not set"), false);
  }

  // get the absolutePath to the wav file
  FString relativePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() + "BouncedWavFiles/" + FileName);
  FString absolutePath = FPaths::ConvertRelativePathToFull(relativePath);

  FString tempHeader = "Bearer ";
  tempHeader += _apiKey;

  // Create the HTTP request
  TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

  // Set the request method, URL, and headers

  HttpRequest->SetURL("https://api.openai.com/v1/audio/transcriptions");
  HttpRequest->SetVerb("POST");

  // ! automate this manual labor!
  // Set the content type, boundary, and form data
  HttpRequest->SetHeader("Authorization", tempHeader);
  HttpRequest->SetHeader("Content-Type", "multipart/form-data; boundary=boundary");
  HttpRequest->SetHeader("model", "whisper-1");
  HttpRequest->SetHeader("language", LanguageISO);
  // HttpRequest->SetHeader("response_format", "verbose_json");
  // HttpRequest->SetHeader("timestamp_granularities", "word");

  TArray<uint8> UpFileRawData;
  FFileHelper::LoadFileToArray(UpFileRawData, *absolutePath);

  FString boundaryStart = "\r\n--boundary\r\n";
  FString contentDispositionFile = FString::Printf(TEXT("Content-Disposition: form-data; name=\"file\"; fileName=\"%s\"\r\n"), *FileName);
  FString contentType = "Content-Type: audio/wav\r\n\r\n";
  FString boundaryMiddle = "\r\n--boundary\r\n";
  FString contentDisposition = "Content-Disposition: form-data; name=\"model\"\r\n\r\n";
  FString transcriptionModel = "whisper-1";
  FString languageContent = "Content-Disposition: form-data; name=\"language\"\r\n\r\n";
  FString languageValue = LanguageISO;
  // FString contentDispositionGranularity = "Content-Disposition: form-data; name=\"timestamp_granularities[]\"\r\n\r\n";
  // FString granularityValue = "word";
  FString boundaryEnd = "\r\n--boundary--\r\n";

  TArray<uint8> data;
  data.Append((uint8*)TCHAR_TO_UTF8(*boundaryStart), boundaryStart.Len());
  data.Append((uint8*)TCHAR_TO_UTF8(*contentDispositionFile), contentDispositionFile.Len());
  data.Append((uint8*)TCHAR_TO_UTF8(*contentType), contentType.Len());
  data.Append(UpFileRawData);

  data.Append((uint8*)TCHAR_TO_UTF8(*boundaryMiddle), boundaryMiddle.Len());
  data.Append((uint8*)TCHAR_TO_UTF8(*contentDisposition), contentDisposition.Len());
  data.Append((uint8*)TCHAR_TO_UTF8(*transcriptionModel), transcriptionModel.Len());

  data.Append((uint8*)TCHAR_TO_UTF8(*boundaryMiddle), boundaryMiddle.Len());
  data.Append((uint8*)TCHAR_TO_UTF8(*languageContent), languageContent.Len());
  data.Append((uint8*)TCHAR_TO_UTF8(*languageValue), languageValue.Len());

  // data.Append((uint8*)TCHAR_TO_UTF8(*boundaryMiddle), boundaryMiddle.Len());
  // data.Append((uint8*)TCHAR_TO_UTF8(*contentDispositionGranularity), contentDispositionGranularity.Len());
  // data.Append((uint8*)TCHAR_TO_UTF8(*granularityValue), granularityValue.Len());
  data.Append((uint8*)TCHAR_TO_UTF8(*boundaryEnd), boundaryEnd.Len());

  HttpRequest->SetContent(data);

  HttpRequest->OnProcessRequestComplete().BindUObject(this, &UOpenAICallTranscriptions::OnResponse);

  HttpRequest->ProcessRequest();
}

void UOpenAICallTranscriptions::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
  if (!WasSuccessful)
  {
    if (bDebug) UE_LOG(LogTemp, Warning, TEXT("Error processing request. \n%s \n%s"), *Response->GetContentAsString(), *Response->GetURL());
    if (Finished.IsBound())
    {
      Finished.Broadcast({}, *Response->GetContentAsString(), false);
    }
    return;
  }

  TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
  TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

  if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
  {
    FString TextValue;
    if (JsonObject->TryGetStringField("text", TextValue))
    {
      if (bDebug) UE_LOG(LogTemp, Log, TEXT("Extracted text: %s"), *TextValue);

      if (JsonObject->TryGetStringField("language", TextValue))
      {
        if (bDebug) UE_LOG(LogTemp, Log, TEXT("Response language: %s"), *TextValue);
      }
    }
    else
    {
      Finished.Broadcast("", "Failed to get 'text' field from JSON response", false);
    }

    Finished.Broadcast(TextValue, "", true);
  }
  else
  {
    Finished.Broadcast("", "Failed to parse JSON response", false);
  }
}
