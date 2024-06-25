#pragma once

#include "CoreMinimal.h"

#include "AudioCapture.h"
#include "Components/ActorComponent.h"
#include "IWebSocket.h"

#include "AudioStreamComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAudioStreamComponent, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogAudioStreamComponentTick, Log, All);

DECLARE_DELEGATE(FAudioDataReceivedDelegate);

UCLASS()
class WEBSOCKETAUDIOSTREAM_API UCustomAudioCapture : public UAudioCapture
{
  GENERATED_BODY()
public:
  bool OpenCustomAudioStream(Audio::FAudioCaptureDeviceParams Params)
  {
    Audio::FOnAudioCaptureFunction OnCapture =
        [this](const void* InAudio, int32 NumFrames, int32 InNumChannels, int32 InSampleRate, double StreamTime, bool bOverFlow) {
          OnGeneratedAudio((const float*)InAudio, NumFrames * InNumChannels);
        };

    if (AudioCapture.OpenAudioCaptureStream(Params, MoveTemp(OnCapture), 1024))
    {
      Init(Params.SampleRate, Params.NumInputChannels);
      return true;
    }

    return false;
  }
};

/* Use this component to stream audio data via websockets connection. */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WEBSOCKETAUDIOSTREAM_API UAudioStreamComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  UAudioStreamComponent();
  virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bDebug;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bLogTick;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bOpenDefaultStream;

  UFUNCTION(BlueprintCallable, Category = "Audio Stream")
  void StartAudioStream();

  UFUNCTION(BlueprintCallable, Category = "Audio Stream")
  void StopAudioStream();

  UFUNCTION(BlueprintCallable, Category = "Audio Stream")
  void PauseAudioStream();

  UFUNCTION(BlueprintCallable, Category = "Audio Stream")
  void ResumeAudioStream();

  void OnAudioGenerate(const float* InAudio, int32 NumSamples);

  UFUNCTION(BlueprintCallable, Category = "Audio Stream")
  void SendAudioData();

  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

  /* The size of audio chunk we sent over websockets. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Stream")
  int32 ChunkSize = 1024;

  /* The target sample rate of audio input stream and websocket compressed data. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Stream")
  int32 SampleRate = 16000;

  /* The target number of channles of audio input stream and websocket compressed data. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Stream")
  int32 NumberOfChannels = 1;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Stream|Websocket")
  FString WebsocketServerProtocol = TEXT("ws");

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Stream|Websocket")
  FString WebsocketServerURL = TEXT("127.0.0.1");
  // docker ip: 172.17.0.2

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio Stream|Websocket")
  FString WebsocketPort = TEXT("9090");

protected:
  // virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
  UPROPERTY()
  UCustomAudioCapture* AudioCapture = nullptr;

  FThreadSafeBool bIsPaused;
  FThreadSafeBool bShouldRun;
  TArray<float> AudioBuffer;
  TArray<uint8> CompressAudioData(const TArray<float>& FloatAudioData);
  void ConvertBytesToFloatArray(const TArray<uint8>& AudioData, TArray<float>& FloatArray);

  TSharedPtr<IWebSocket> Socket;
  void Send(const TArray<uint8>& Data);
  FOnAudioGenerate OnAudioGenerateDelegate;
};
