// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/Public/TimeoutManager.h"

#include "AiIntellectComponent.h"
#include "Api/Models/Conversation.h"
#include "AvatarPawn.h"
#include "AvatarsTypes.h"

#include "AvatarsPlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAwatarsPlayerController, Display, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLanguageChangeSignature, EAvatarLanguage, Language);

class IWebSocket;
class URootWidget;
class URuntimeAudioExporter;

UCLASS()
class AVATARS_API AAvatarsPlayerController : public APlayerController
{
  GENERATED_BODY()

public:
  AAvatarsPlayerController();
  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  static AAvatarsPlayerController* Get(UWorld* World);

  /* The menu should be accessible to service stuff only, the service pin helps to hide the commands funcionality behind password. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Service")
  FString ServicePin = "1234";

  /* The menu should be accessible to service stuff only, and once opened it should automatically close after short time to prevent it from
   * being visible to users. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Service")
  float MenuTimeout = 30.0f;

  UFUNCTION(BlueprintCallable, Category = "Input")
  void OnAnyKey();

  /* If set this timer will switch avatars after each timeout. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FTimeoutManager ChangeAvatarTimeout{0.0f, true, "Change Avatar Timeout"};
  UFUNCTION() void StartChangeAvatarTimeout();
  UFUNCTION() void OnAvatarChangeTimeout();

  /* If set this timer will play greeting dialog after each timeout. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FTimeoutManager IdleGreetingTimeout{0.0f, true, "Idle Greeting Timeout"};
  UFUNCTION() void StartGreetingTimeout();
  UFUNCTION() void OnGreetingTimeout();

  /* Use this timeout to limit the time the AI services have to respond to users inputs. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FTimeoutManager ThinkingTimeout{5.0f, true, "Thinking Timeout"};
  UFUNCTION() void StartThinkingTimeout();
  UFUNCTION() void OnThinkingTimeout();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller")
  bool bSendOnlyOnFinal = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller")
  bool bDebug = false;

  /* This method should return either avatar with bIsDefault set to true or the first avatar found on the map. */
  UFUNCTION(BlueprintCallable, Blueprintpure)
  AAvatarPawn* GetDefaultAvatar() const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller")
  bool bShowRuntimeProperties = false;

  /* When checked every user message will displayed along with recorded audio and transcription. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|User Interface")
  bool bShowUserMessage = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|User Interface")
  float HideUserMessageDelay = 2.0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|User Interface")
  TSubclassOf<URootWidget> RootWidgetClass;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller", meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides))
  URootWidget* RootWidget;

  UFUNCTION(BlueprintCallable)
  void StartNewConversation(const bool bDisplayAvatarResponse = false);

  UFUNCTION(BlueprintCallable, Blueprintpure)
  bool StartedConversation() const;

  // Create root widget and return false if there was an error.
  bool CreateRootWidget();
  void ScheduleHideLoadingScreen();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|User Interface")
  EAvatarLanguage Language = EAvatarLanguage::PL;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|User Interface")
  bool bShowAvatarMessage;

  UFUNCTION(BlueprintCallable)
  void SetLanguage(EAvatarLanguage InLanguage);

  UPROPERTY(BlueprintAssignable)
  FOnLanguageChangeSignature OnLanguageChangeEvent;

  UFUNCTION(BlueprintCallable, Blueprintpure)
  FString GetLanguageAsApiString();

  UFUNCTION(BlueprintCallable, Blueprintpure)
  FString GetLanguageAsLocalString();

  UFUNCTION(BlueprintCallable, Blueprintpure)
  FString GetLanguageAsIsoString();

  UFUNCTION(BlueprintCallable)
  void StopDialog();

  UFUNCTION()
  void OnDialogStopped();

  UFUNCTION(BlueprintCallable, Category = "Avatars Player Controller|Camera")
  void OnCameraMoveInput(const FVector2D& Movement);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Camera")
  float CameraZoomSpeed = 400.f;

  /* Use value between 0 and 1 to scale zoom between min and max values/ */
  UFUNCTION(BlueprintCallable)
  void SetCameraZoomByAlpha(const float Alpha);

  UFUNCTION(BlueprintCallable, Category = "Avatars Player Controller|Camera")
  void OnCameraZoomInput(const float Zoom);

  UFUNCTION(BlueprintCallable, Category = "Avatars Player Controller|Camera")
  void ResetCameraLocation();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller")
  float InitialLoadDelay = 2.f;

  // List of spawned avatars, that is populated when the game starts.
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<AAvatarPawn*> SpawnedAvatars;

  UFUNCTION(BlueprintCallable)
  AAvatarPawn* GetNextAvatar();

  /**
   * When this is set to true starting new conversation witll be followed by playing greeting dialog.
   * Set this to false if the initial greeting dialog should not be played
   **/
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller")
  bool bPlayInitialDialog;

  UPROPERTY()
  TArray<FString> PendingMessages;

  /* Send user's text message to AI agent. Returns true if the message wasn't empty,
  was correctly formated and sent while false otherwise. */
  UFUNCTION(BlueprintCallable, meta = (ReturnValueDisplayName = "Message Sent"))
  bool SendUserMessage(FString Message);

  /* The multi part dialog consist of number of small dialogs. */
  UFUNCTION(BlueprintCallable)
  void DisplayAvatarMessage(FString Message, FString AudioPath, const TArray<FString>& NewSuggestions, const TArray<FString>& ResponseTags);

  UPROPERTY()
  TArray<FString> Suggestions;

  UFUNCTION(BlueprintCallable)
  void OnCharacterSelection(AActor* SelectedActor);

  UFUNCTION(BlueprintCallable, Blueprintpure)
  AAvatarPawn* GetSelectedAvatar() const;

  UFUNCTION(BlueprintCallable)
  void SetAvatarState(EAvatarState State);

  UFUNCTION(BlueprintCallable, Blueprintpure)
  bool GetActiveAvatarState(EAvatarState& OutState);

  UFUNCTION()
  void OnAvatarStateChanged(EAvatarState OldState, EAvatarState NewState);

  /* Websocket aimed at connecting with dockerised Whisper server. */
  FTimerHandle ReconnectTimerHandle;

  TSharedPtr<IWebSocket> WhisperWebsocket;
  void SetupWhisperWebsockets();
  void CleanupWebSocket();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Transcription")
  TArray<FString> ExcludedPhrases;

  UFUNCTION(BlueprintCallable)
  bool IsExcludedPhrase(const FString& Phrase) const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Transcription")
  bool bUseLocalTranscription = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Transcription")
  FString WhisperWebsocketServerProtocol = TEXT("ws");

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Transcription")
  FString WhisperWebsocketServerURL = TEXT("127.0.0.1");
  // docker ip: 172.17.0.2

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Transcription")
  FString WhisperWebsocketPort = TEXT("9090");

  /* This text is what is being sent to the whisper server on successful connection. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Transcription")
  FString OnConnectedResponse = FString("single_client");

  /* This text is what is being sent to the whisper server on successful connection. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Transcription")
  float Treshold = 0.87;

  UFUNCTION(Blueprintcallable)
  FString ConnectToWhisperServer();

  /* Use this to send clear request to the Whisper server to clear its transcription memory and
   * context. */
  UFUNCTION(Blueprintcallable)
  void ClearSpeechToText();

  UFUNCTION(Blueprintcallable)
  void EnableAudioCapture();
  float AudioCaptureTimer = 0.0f;
  bool bAudioCaptureBelowMinDuration = false;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Audio Capture")
  float MinAudioCaptureDuration = 0.4f;

  UFUNCTION(Blueprintcallable)
  void DisableAudioCapture();
};
