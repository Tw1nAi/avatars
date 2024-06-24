// Copyright Juice sp. z o. o., All Rights Reserved.

#include "AudioStreamComponent.h"

#include "Async/Async.h"
#include "AudioDeviceManager.h"
#include "AudioMixerDevice.h"
#include "WebSocketsModule.h"

DEFINE_LOG_CATEGORY(LogAudioStreamComponent);
DEFINE_LOG_CATEGORY(LogAudioStreamComponentTick);

UAudioStreamComponent::UAudioStreamComponent()
{
  PrimaryComponentTick.bCanEverTick = true;
  bShouldRun = false;
  bIsPaused = false;
  ChunkSize = 1024;
  SampleRate = 16000;
}

void UAudioStreamComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  StopAudioStream();
}

void UAudioStreamComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{

  StopAudioStream();

  Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UAudioStreamComponent::OnAudioGenerate(const float* InAudio, int32 NumSamples)
{
  AudioBuffer.Append(InAudio, NumSamples);
}

void UAudioStreamComponent::StartAudioStream()
{
  FString Url = WebsocketServerProtocol + "://" + WebsocketServerURL + ":" + WebsocketPort + "/";
  Socket = FWebSocketsModule::Get().CreateWebSocket(Url, WebsocketServerProtocol);

  if (!Socket.IsValid())
  {
    if (bDebug) UE_LOG(LogAudioStreamComponent, Error, TEXT("Failed to create a websocket connection."));
    return;
  }

  Socket->OnConnected().AddLambda([this]() {
    UE_LOG(LogAudioStreamComponent, Display, TEXT("Audio Stream Component Websocket Connected!"));
    this->Socket->Send("open_audio_stream");
  });
  Socket->OnMessage().AddLambda([](const FString& Message) {
    UE_LOG(LogAudioStreamComponent, Display, TEXT("[INFO] Received: %s"), *Message);
  });
  Socket->OnConnectionError().AddLambda([this](const FString& Error) {
    UE_LOG(LogAudioStreamComponent, Error, TEXT("[ERROR] Received: %s"), *Error);
  });
  Socket->OnClosed().AddLambda([](int32 StatusCode, const FString& Reason, bool bWasClean) {
    UE_LOG(LogAudioStreamComponent, Error, TEXT("Closed: %d, %s, %d"), StatusCode, *Reason, bWasClean);
  });
  Socket->Connect();

  AudioCapture = NewObject<UCustomAudioCapture>();

  if (bOpenDefaultStream && !AudioCapture->OpenDefaultAudioStream())
  {
    UE_LOG(LogAudioStreamComponent, Error, TEXT("[ERROR] Failed to open a default audio stream to the audio capture device."));
    return;
  }

  OnAudioGenerateDelegate =
      FOnAudioGenerate([this](const float* InAudio, int32 NumSamples) mutable { this->OnAudioGenerate(InAudio, NumSamples); });
  AudioCapture->AddGeneratorDelegate(OnAudioGenerateDelegate);

  Audio::FAudioCaptureDeviceParams Params;
  Params.SampleRate = SampleRate;
  Params.NumInputChannels = NumberOfChannels;
  if (!AudioCapture->OpenCustomAudioStream(Params))
  {
    UE_LOG(LogAudioStreamComponent, Error, TEXT("Failed to open a default audio stream to the audio capture device."));
    return;
  }
  AudioCapture->StartCapturingAudio();
  bShouldRun = true;
}

void UAudioStreamComponent::StopAudioStream()
{
  bShouldRun = false;
  if (AudioCapture)
  {
    AudioCapture->StopCapturingAudio();
    AudioCapture = nullptr;
  }
  if (Socket.IsValid() && Socket->IsConnected())
  {
    Socket->Close();
  }
}

void UAudioStreamComponent::PauseAudioStream()
{
  bIsPaused = true;
}

void UAudioStreamComponent::ResumeAudioStream()
{
  bIsPaused = false;
}

TArray<uint8> UAudioStreamComponent::CompressAudioData(const TArray<float>& FloatAudioData)
{
  TArray<uint8> CompressedData;
  CompressedData.Reserve(FloatAudioData.Num() * sizeof(float));
  for (float Sample : FloatAudioData)
  {
    Sample = FMath::Clamp(Sample, -1.0f, 1.0f);
    CompressedData.Append(reinterpret_cast<const uint8*>(&Sample), sizeof(float));
  }
  return CompressedData;
}

void UAudioStreamComponent::SendAudioData()
{
  if (AudioBuffer.Num() >= ChunkSize && Socket.IsValid() && Socket->IsConnected())
  {
    TArray<float> DataToSend;
    DataToSend.Append(AudioBuffer.GetData(), ChunkSize);
    AudioBuffer.RemoveAt(0, ChunkSize);
    TArray<uint8> CompressedData = CompressAudioData(DataToSend);
    Socket->Send(CompressedData.GetData(), CompressedData.Num(), true);
  }
}

void UAudioStreamComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
  UE_LOG(
      LogAudioStreamComponentTick,
      Verbose,
      TEXT("AudioBuffer.Num() >= ChunkSize: %s"),
      AudioBuffer.Num() >= ChunkSize ? TEXT("true") : TEXT("false")
  );
  UE_LOG(LogAudioStreamComponentTick, Verbose, TEXT("Socket.IsValid(): %s"), Socket && Socket.IsValid() ? TEXT("true") : TEXT("false"));
  UE_LOG(
      LogAudioStreamComponentTick,
      Verbose,
      TEXT("Socket->IsConnected(): %s"),
      Socket && Socket->IsConnected() ? TEXT("true") : TEXT("false")
  );

  if (bShouldRun) SendAudioData();
}

void UAudioStreamComponent::Send(const TArray<uint8>& Data)
{
  if (Socket.IsValid() && Socket->IsConnected())
  {
    Socket->Send((char*)Data.GetData(), Data.Num());
  }
}

void UAudioStreamComponent::ConvertBytesToFloatArray(const TArray<uint8>& AudioData, TArray<float>& FloatArray)
{
  int32 NumSamples = AudioData.Num() / sizeof(int16); // Two bytes per sample for 16-bit audio
  const int16* Samples = reinterpret_cast<const int16*>(AudioData.GetData());

  FloatArray.Empty();
  FloatArray.Reserve(NumSamples);

  // Convert each 16-bit integer sample to a floating point in the range of -1.0 to 1.0
  for (int32 i = 0; i < NumSamples; ++i)
  {
    float SampleValue = (float)Samples[i] / 32768.0f; // 32768.0f is the max value for int16
    FloatArray.Add(SampleValue);
  }
}