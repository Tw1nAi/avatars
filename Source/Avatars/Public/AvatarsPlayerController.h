// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/Public/TimeoutManager.h"

#include "AvatarPawn.h"
#include "AvatarsApi/Api_v1/Models/Avatar.h"
#include "AvatarsApi/Api_v2/Models/Conversation_v2.h"
#include "AvatarsTypes.h"

#include "AvatarsPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLanguageChangeSignature, EAvatarLanguage, Language);

class IWebSocket;
class UAvatarsApi;
class UAvatarsApi_v2;
class UDataTable;
class URootWidget;
class URuntimeAudioExporter;

UCLASS()
class AVATARS_API AAvatarsPlayerController : public APlayerController
{
  GENERATED_BODY()

public:
  AAvatarsPlayerController();
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaSeconds) override;
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

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller")
  EAvatarCharacter DefaultAvatar = EAvatarCharacter::WojtekTheBear;

  UFUNCTION(BlueprintCallable, Blueprintpure)
  const FAvatarData& GetDefaultAvatarData() const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller", meta = (DisplayAfter = "AvatarsDataTable"))
  bool bShowRuntimeProperties = false;

  /* When checked every user message will displayed along with recorded audio and transcription. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|User Interface")
  bool bDisplayUserMessage = true;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|User Interface")
  float HideUserMessageDelay = 2.0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|User Interface")
  TSubclassOf<URootWidget> RootWidgetClass;

  UPROPERTY(
      VisibleAnywhere,
      BlueprintReadWrite,
      Category = "Avatars Player Controller",
      meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides, DisplayAfter = "AvatarsDataTable")
  )
  URootWidget* RootWidget;

  void SetupWidgets();
  void LoadAvatarsData();
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

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller")
  UDataTable* AvatarsDataTable;

  UPROPERTY(
      VisibleAnywhere,
      BlueprintReadWrite,
      Category = "Avatars Player Controller",
      meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides, DisplayAfter = "AvatarsDataTable")
  )
  TArray<FAvatarData> Avatars;
  UFUNCTION() FAvatarData GetNextAvatar();

  UFUNCTION(BlueprintCallable)
  AAvatarPawn* SpawnAvatar(TSubclassOf<AAvatarPawn> AvatarClass);

  UFUNCTION(BlueprintCallable)
  void OnAvatarsDataReceived();

  UFUNCTION(BlueprintCallable)
  void StartNewConversation(const bool bDisplayAvatarResponse);

  UFUNCTION(BlueprintCallable, Blueprintpure)
  bool StartedConversation();

  /**
   * When this is set to true starting new conversation witll be followed by playing greeting dialog.
   * Set this to false if the initial greeting dialog should not be played
   **/
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller")
  bool bPlayInitialDialog;

  UPROPERTY()
  TArray<FString> PendingMessages;

  UPROPERTY(
      VisibleAnywhere,
      BlueprintReadWrite,
      Category = "Avatars Player Controller",
      meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides)
  )
  int32 ConversationId = -1;

  UPROPERTY(
      VisibleAnywhere,
      BlueprintReadWrite,
      Category = "Avatars Player Controller",
      meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides)
  )
  FString ConversationId_v2 = "";

  UFUNCTION(BlueprintCallable)
  void SetConversationId(int32 Id);

  UFUNCTION(BlueprintCallable)
  void SetConversationId_v2(FString Id);

  /* Send user's text message to AI agent. Returns true if the message wasn't empty, was correctly formated and sent and false otherwise. */
  UFUNCTION(BlueprintCallable, meta = (ReturnValueDisplayName = "Message Sent"))
  bool SendUserMessage(FString Message);

  UFUNCTION(BlueprintCallable)
  void DisplayAvatarMessage(FAvatarMessage Message);

  UFUNCTION(BlueprintCallable)
  void DisplayAvatarMessage_v2(FMessage_v2 Message, const TArray<FString>& NewSuggestions);

  UPROPERTY()
  TArray<FString> Suggestions;

  UFUNCTION(BlueprintCallable)
  void OnCharacterSelection(const FAvatarId& AvatarId);

  UFUNCTION(BlueprintCallable, Blueprintpure)
  AAvatarPawn* GetSelectedAvatar() const;

  UFUNCTION(BlueprintCallable, Blueprintpure)
  const FAvatarId& GetSelectedAvatarId() const;

  UFUNCTION(BlueprintCallable, Blueprintpure)
  FAvatarData& GetSelectedAvatarData();

  UFUNCTION(BlueprintCallable, Blueprintpure)
  bool GetActiveAvatarState(EAvatarState& OutState);

  UFUNCTION()
  void OnAvatarStateChanged(EAvatarState OldState, EAvatarState NewState);

  /* Websocket aimed at connecting with dockerised Whisper server. */
  FTimerHandle ReconnectTimerHandle;

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Whisper Websocket")
  UAvatarsApi* Api;
  void SetupApi_v1();

  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Whisper Websocket")
  UAvatarsApi_v2* Api_v2;
  void SetupApi_v2();

  TSharedPtr<IWebSocket> WhisperWebsocket;
  void SetupWhisperWebsockets();
  void CleanupWebSocket();

  // ! temporary bool to deal with two APIs reaching for information. The one that finishes
  // ! this race first sets this as true and the other one that sees that it's true can init
  // ! post fetch operations. Remove once API v1 and API v2 are consolidated into one.
  UPROPERTY()
  bool bApiDone = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Transcription")
  TArray<FString> ExcludedPhrases;

  bool IsExcludedPhrase(const FString Phrase);

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
  void ClearSpeachToText();

  UFUNCTION(Blueprintcallable)
  void EnableAudioCapture();

  UFUNCTION(Blueprintcallable)
  void DisableAudioCapture();
};
