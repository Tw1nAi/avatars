// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "Interfaces/IHttpRequest.h"
#include "JsonObjectConverter.h"
#include "UObject/NoExportTypes.h"

#include "RestApi.generated.h"

class IHttpRequest;
class FHttpModule;

USTRUCT(Blueprintable, BlueprintType)
struct FHttpHeader
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Name;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Value;
};

// Template class for handling HTTP requests and processing their responses.
// Allows for sending requests and chaining response handling with a fluent interface.
template <typename InResponseType, typename InPayloadType = InResponseType>
class AVATARS_API TRequestHandle
{
public:
  // Shared pointer to the HTTP request, ensuring thread safety.
  TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request;

  // Executes the HTTP request without a payload.
  void Run()
  {
    Request->ProcessRequest();
  }

  // Executes the HTTP request with a payload, converting the payload to a JSON string.
  void Run(const InPayloadType& RequestData)
  {
    FString RequestBody;
    if (FJsonObjectConverter::UStructToJsonObjectString(RequestData, RequestBody))
    {
      Request->SetContentAsString(RequestBody);
    }

    Request->ProcessRequest();
  }

  // Binds a callback to execute when the request completes, converting the JSON response back to a struct.
  TRequestHandle<InResponseType, InPayloadType>* Then(TFunction<void(InResponseType)> OnJsonConverted)
  {
    Request->OnProcessRequestComplete().BindLambda([OnJsonConverted](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
      const bool bRequestSucceeded = bWasSuccessful && Response.IsValid();

      if (!bRequestSucceeded)
      {
        verify(!bRequestSucceeded);
        // TODO: Handle HTTP request failure
        return;
      }

      InResponseType ResponseData;
      const bool bJsonConversionSucceeded = FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &ResponseData, 0, 0);

      if (!bJsonConversionSucceeded)
      {
        verify(!bJsonConversionSucceeded);
        // TODO: Handle JSON conversion failure
        return;
      }
      OnJsonConverted(ResponseData);
    });

    return this;
  }
};

/**
 * Base REST API class that allows to run regular REST calls.
 */
UCLASS(Blueprintable, BlueprintType)
class AVATARS_API URestApi : public UObject
{
  GENERATED_BODY()

public:
  URestApi();

  inline static constexpr TCHAR* Get = TEXT("GET");
  inline static constexpr TCHAR* Post = TEXT("POST");
  inline static constexpr TCHAR* Put = TEXT("PUT");
  inline static constexpr TCHAR* Delete = TEXT("DELETE");

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString BaseUrl;

  UFUNCTION(BlueprintCallable)
  void SetBaseUrl(const FString InBaseUrl = TEXT(""));

  UFUNCTION(BlueprintCallable)
  void OnError(FString ErrorMessasge);

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bDebug = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FHttpHeader> DefaultHeaders;

  UFUNCTION(BlueprintCallable)
  void AddDefaultHeader(const FString Name, const FString Value);

  template <typename InResponseType, typename InPayloadType = InResponseType>
  TSharedRef<TRequestHandle<InResponseType, InPayloadType>, ESPMode::ThreadSafe> MakeHandle()
  {
    return MakeShared<TRequestHandle<InResponseType, InPayloadType>, ESPMode::ThreadSafe>();
  }

  template <typename InResponseType, typename InPayloadType = InResponseType>
  TSharedRef<TRequestHandle<InResponseType, InPayloadType>, ESPMode::ThreadSafe>& CreateHttpRequest(TSharedRef<TRequestHandle<InResponseType, InPayloadType>>& Handle, const FString& Verb, const FString& Url = TEXT(""), const InPayloadType* RequestData = nullptr)
  {
    auto HttpRequest = FHttpModule::Get().CreateRequest();
    Handle->Request = HttpRequest;

    HttpRequest->SetURL(BaseUrl + Url);
    HttpRequest->SetVerb(Verb);

    for (FHttpHeader& Header : DefaultHeaders)
    {
      HttpRequest->SetHeader(Header.Name, Header.Value);
    }

    if (RequestData != nullptr && (Verb == URestApi::Post || Verb == URestApi::Put))
    {
      FString RequestBody;
      if (FJsonObjectConverter::UStructToJsonObjectString(*RequestData, RequestBody))
      {
        HttpRequest->SetContentAsString(RequestBody);
      }
    }

    return Handle;
  }
};
