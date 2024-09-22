
// Copyright Juice sp. z o. o., All Rights Reserved.

#include "AvatarsPlayerController.h"

#include "Components/AudioComponent.h"
#include "GameFramework/GameUserSettings.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"

#include "AiIdentity.h"
#include "AiIntellectComponent.h"
#include "AvatarPawn.h"
#include "AvatarsGame.h"
#include "Get.h"
#include "Log.h"
#include "MakeJsonString.h"
#include "MovieSceneSequenceID.h"
#include "Persistance/PersistanceController.h"
#include "RestApi.h"
#include "TextHelpers.h"
#include "UserInterface/RootWidget.h"

DEFINE_LOG_CATEGORY(LogAwatarsPlayerController);

AAvatarsPlayerController::AAvatarsPlayerController()
{
  IdleGreetingTimeout.TimeoutDelay = 30.0f;

  // add the default excluded phrases
  ExcludedPhrases.Add(TEXT("KONIEC!"));
  ExcludedPhrases.Add(TEXT("KONIEC"));
  ExcludedPhrases.Add(TEXT("PARROT"));
  ExcludedPhrases.Add(TEXT("PARROT TV"));
  ExcludedPhrases.Add(TEXT("support on Patronite"));
  ExcludedPhrases.Add(TEXT("Thank you for your support on Patronite"));
  ExcludedPhrases.Add(TEXT("Zapraszam na kolejny odcinek"));
  ExcludedPhrases.Add(TEXT("Amara.org"));
  ExcludedPhrases.Add(TEXT("Wszystkie prawa zastrzeżone"));
  ExcludedPhrases.Add(TEXT("Dzięki za uwagę"));
  ExcludedPhrases.Add(TEXT("Dzięki za obejrzenie"));
  ExcludedPhrases.Add(TEXT("Świerczek-Gryboś"));
  ExcludedPhrases.Add(TEXT("Transkrypcja Magdalena Świerczek-Gryboś"));
  ExcludedPhrases.Add(TEXT("PTA – TVP"));
  ExcludedPhrases.Add(TEXT("do zobaczenia w kolejnym odcinku"));
  ExcludedPhrases.Add(TEXT("Polskie Towarzystwo Astronomiczne"));
  ExcludedPhrases.Add(TEXT("Towarzystwo Astronomiczne"));
  ExcludedPhrases.Add(TEXT("Telewizja Polska"));
  ExcludedPhrases.Add(TEXT("nie zapomnijcie zasubskrybować"));
  ExcludedPhrases.Add(TEXT("zafollowować"));
  ExcludedPhrases.Add(TEXT("zafollowować mnie na Facebooku"));
  ExcludedPhrases.Add(TEXT("Zdjęcia i montaż"));
  ExcludedPhrases.Add(TEXT("Pracownia Prawa i Sprawiedliwość"));
  ExcludedPhrases.Add(TEXT("Teraz wracamy do naszego odcinka"));
  ExcludedPhrases.Add(TEXT("wracamy do naszego odcinka"));
  ExcludedPhrases.Add(TEXT("najnowszych filmików na świecie"));
  ExcludedPhrases.Add(TEXT("Dziękuje za oglądanie i do zobaczenia w kolejnych odcinkach!"));
}

AAvatarsPlayerController* AAvatarsPlayerController::Get(UWorld* World)
{
  if (ULog::ErrorIf(World == nullptr, "World is nullptr")) return nullptr;

  UGameInstance* GameInstance = World->GetGameInstance();
  if (ULog::ErrorIf(GameInstance == nullptr, "GameInstance is nullptr")) return nullptr;

  APlayerController* Controller = GameInstance->GetFirstLocalPlayerController(World);
  if (Controller == nullptr)
  {
    // ! find out why this is happening
    ULog::Error("Could not get reference to AAvatarsPlayerController* PlayerController");
    return nullptr;
  }

  AAvatarsPlayerController* OutController = Cast<AAvatarsPlayerController>(Controller);
  if (OutController == nullptr)
  {
    // ! find out why this is happening
    ULog::Error("Could not cast to AAvatarsPlayerController");
    return nullptr;
  }

  return OutController;
}

void AAvatarsPlayerController::BeginPlay()
{
  Super::BeginPlay();

  if (UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings())
  {
    GameUserSettings->ApplySettings(false);
  }

  // TODO : move this to AAvatarPawn an init when the avatar is created or possesed
  if (UPersistanceController* Persistance = UAvatarsGame::GetPersistance(GetWorld()))
  {
    ChangeAvatarTimeout.SetTimeoutDelay(Persistance->ConversationSettings.AvatarChangeTimeout);
    if (ChangeAvatarTimeout.IsReady()) StartChangeAvatarTimeout();
    IdleGreetingTimeout.SetTimeoutDelay(Persistance->ConversationSettings.IdleGreetingTimeout);
    if (IdleGreetingTimeout.IsReady()) StartGreetingTimeout();
  }

  ChangeAvatarTimeout.bDebug = bDebug;
  IdleGreetingTimeout.bDebug = bDebug;
  ThinkingTimeout.bDebug = bDebug;

  DisableInput(this);
  if (!CreateRootWidget()) return;

  TArray<AActor*> SpawnedAvatarsActors;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAvatarPawn::StaticClass(), SpawnedAvatarsActors);
  for (AActor* Actor : SpawnedAvatarsActors)
  {
    AAvatarPawn* Avatar = Cast<AAvatarPawn>(Actor);
    if (Avatar != nullptr)
    {
      SpawnedAvatars.Add(Avatar);
    }
  }

  if (SpawnedAvatars.Num() == 0)
  {
    FString WarningMessage = "No avatars found in the scene.";
    UE_LOG(LogAwatarsPlayerController, Warning, TEXT("%s"), *WarningMessage);
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, WarningMessage);
    return;
  }

  TArray<AActor*> Removed;
  SpawnedAvatars.RemoveAll([&Removed](AAvatarPawn* Avatar) {
    if (!Avatar->bEnabled)
    {
      Removed.Add(Avatar);
      return true;
    };
    return false;
  });
  for (AActor* Actor : Removed)
  {
    Actor->Destroy();
  }

  if (SpawnedAvatars.Num() > 1)
  {
    SpawnedAvatars.Sort([](const AAvatarPawn& A, const AAvatarPawn& B) { return A.Order < B.Order; });
    RootWidget->CreateAvatarsThumbnails(SpawnedAvatars);
  }
  else
  {
    RootWidget->SetCharacterSelectionVisibility(false);
  }

  OnCharacterSelection(GetDefaultAvatar());
  ScheduleHideLoadingScreen();

  if (bUseLocalTranscription)
  {
    SetupWhisperWebsockets();
    ConnectToWhisperServer();
  }
}

void AAvatarsPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  CleanupWebSocket();
}

void AAvatarsPlayerController::SetUseLocalTranscription(bool bInUseLocalTranscription)
{
  if (bInUseLocalTranscription == bUseLocalTranscription) return;

  bUseLocalTranscription = bInUseLocalTranscription;

  if (bUseLocalTranscription)
  {
    SetupWhisperWebsockets();
    ConnectToWhisperServer();
  }
  else
  {
    CleanupWebSocket();
  }

  UE_LOG(LogAwatarsPlayerController, Display, TEXT("bUseLocalTranscription: %s"), bUseLocalTranscription ? TEXT("true") : TEXT("false"));
}
void AAvatarsPlayerController::StartNewConversation(const bool bDisplayAvatarResponse)
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return;

  Avatar->IntellectComponent->StartNewConversation(GetLanguageAsIsoString());
}

bool AAvatarsPlayerController::StartedConversation() const
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return false;

  return Avatar->IntellectComponent->HasStartedConversation();
}

bool AAvatarsPlayerController::CreateRootWidget()
{
  if (RootWidgetClass == nullptr)
  {
    FString ErrorMessage = "RootWidgetClass is not set in AAvatarsPlayerController";
    UE_LOG(LogAwatarsPlayerController, Error, TEXT("%s"), *ErrorMessage);
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, ErrorMessage);
    return false;
  }

  UUserWidget* Widget = CreateWidget<UUserWidget>(this, RootWidgetClass);
  if (Widget == nullptr)
  {
    FString ErrorMessage = "Could not create RootWidgetClass in AAvatarsPlayerController";
    UE_LOG(LogAwatarsPlayerController, Error, TEXT("%s"), *ErrorMessage);
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, ErrorMessage);
    return false;
  }

  Widget->AddToViewport();
  RootWidget = Cast<URootWidget>(Widget);
  RootWidget->AddToViewport();
  RootWidget->OnCharacterThumbnailClickEvent.AddUObject(this, &AAvatarsPlayerController::OnCharacterSelection);
  // RootWidget->bDebug = bDebug;

  return true;
}

void AAvatarsPlayerController::ScheduleHideLoadingScreen()
{
  FTimerHandle Handle;
  auto HideLoadingScreen = FTimerDelegate::CreateLambda([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)] {
    if (ULog::ErrorIf(!WeakController.IsValid(), "AvatarsPlayerController this reference is not valid AvatarsPlayerController BeginPlay HideLoadingScreen lambda")) return;
    WeakController->EnableInput(WeakController.Get());
    WeakController->RootWidget->HideLoadingScreen();
  });
  GetWorldTimerManager().SetTimer(Handle, HideLoadingScreen, InitialLoadDelay, false);
}

void AAvatarsPlayerController::SetupWhisperWebsockets()
{
  if (WhisperWebsocket != nullptr) return;

  FString Url = WhisperWebsocketServerProtocol + "://" + WhisperWebsocketServerURL + ":" + WhisperWebsocketPort + "/";

  // if (bDebug) GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, Url);

  WhisperWebsocket = FWebSocketsModule::Get().CreateWebSocket(Url, WhisperWebsocketServerProtocol);

  /* On Connected */
  WhisperWebsocket->OnConnected().AddLambda([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)]() mutable {
    if (ULog::ErrorIf(!WeakController.IsValid(), "AvatarsPlayerController this reference is not valid in WhisperWebsocket->OnConnected lambda")) return;

    ULog::InfoIf(WeakController->bDebug, "Connected to WhisperWebsocket");
    WeakController->WhisperWebsocket->Send(WeakController->OnConnectedResponse);
  });

  /* On Error */
  WhisperWebsocket->OnConnectionError().AddLambda([](const FString& Error) mutable { ULog::Error("Websockets connection error: " + Error); });

  /* On Closed */
  WhisperWebsocket->OnClosed().AddLambda([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)](int32 StatusCode, const FString& Reason, bool bWasClean) mutable {
    if (ULog::ErrorIf(!WeakController.IsValid(), "AvatarsPlayerController this reference is not valid in WhisperWebsocket->OnClosed lambda")) return;

    ULog::InfoIf(
        WeakController->bDebug,
        "Closed WhisperWebsocket connection. Reason: " + Reason + " StatusCode: " + FString::FromInt(StatusCode) + " bWasClean: " + (bWasClean ? "true" : "false")
    );
  });

  /* On Message */
  WhisperWebsocket->OnMessage().AddLambda([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)](const FString& Message) mutable {
    if (ULog::ErrorIf(!WeakController.IsValid(), "AvatarsPlayerController this reference is not valid in WhisperWebsocket->OnMessage lambda.")) return;

    if (WeakController->bAudioCaptureBelowMinDuration)
    {
      WeakController->bAudioCaptureBelowMinDuration = false;
      const FString InfoMessage = "Audio capture duration is below the minimum duration.";
      UE_LOG(LogAwatarsPlayerController, Display, TEXT("%s"), *InfoMessage);
      // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, InfoMessage);
      AAvatarPawn* Avatar = WeakController->GetSelectedAvatar();
      if (Avatar != nullptr)
      {
        Avatar->SetState(EAvatarState::Idle);
      }
      return;
    }

    if (WeakController->bDebug)
    {
      // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, Message);
    }

    if (WeakController->IsExcludedPhrase(Message))
    {
      if (WeakController->bDebug)
      {
        FString DebugMessage = "Detected excluded phrase in user message: " + Message;
        // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Orange, DebugMessage);
        UE_LOG(LogAwatarsPlayerController, Warning, TEXT("%s"), *DebugMessage);
        if (AAvatarPawn* Avatar = WeakController->GetSelectedAvatar())
        {
          Avatar->SetState(EAvatarState::Idle);
        }
      }
      return;
    }

    if (!WeakController->bSendOnlyOnFinal || Message.Contains("[FINAL]"))
    {
      WeakController->ThinkingTimeout.ClearIfValid(WeakController->GetWorld());

      FString CleanMessage = Message;
      CleanMessage.ReplaceInline(TEXT("[FINAL]"), TEXT(""), ESearchCase::IgnoreCase);
      CleanMessage.TrimStartAndEndInline();

      const bool bWasSent = WeakController->SendUserMessage(CleanMessage);
      AAvatarPawn* Avatar = WeakController->GetSelectedAvatar();
      if (Avatar != nullptr)
      {
        Avatar->SetState(bWasSent ? EAvatarState::Thinking : EAvatarState::Idle);
      }

      if (WeakController->bShowUserMessage && WeakController->RootWidget != nullptr)
      {
        WeakController->RootWidget->ShowUserMessage(CleanMessage, FTextHelpers::CalculateReadingTime(CleanMessage));
      }
    }
  });

  // ! this is for debugging only, should not be needed in shipped product since the server should
  // ! be running before the app starts or during its loading phase
  /* Run timer that will tick at interval to check if websocket is connected. If not try to connect. */
  // GetWorldTimerManager().SetTimer(
  //     ReconnectTimerHandle,
  //     this,
  //     &AAvatarsPlayerController::ConnectToWhisperServer,
  //     1.0f,
  //     true);
}

void AAvatarsPlayerController::CleanupWebSocket()
{
  if (ReconnectTimerHandle.IsValid())
  {
    ReconnectTimerHandle.Invalidate();
  }

  if (WhisperWebsocket.IsValid())
  {
    WhisperWebsocket->Close(1000, TEXT("Unreal Engine exited."));
  }
  WhisperWebsocket.Reset(); // reset the access count and allow to GC
}

FString AAvatarsPlayerController::ConnectToWhisperServer()
{
  if (!WhisperWebsocket.IsValid()) return "WhisperWebsocket instance is nullptr";

  if (!WhisperWebsocket->IsConnected())
  {
    WhisperWebsocket->Connect();
    return "WhisperWebsocket is not connected. Trying to connect to WhisperWebsocket";
  }

  // GetWorldTimerManager().PauseTimer(this->ReconnectTimerHandle);
  return "WhisperWebsocket is connected.";
}

void AAvatarsPlayerController::ClearSpeechToText()
{
  if (WhisperWebsocket.IsValid())
  {
    WhisperWebsocket->Send("clear");
  }
}

void AAvatarsPlayerController::EnableAudioCapture()
{
  if (ULog::ErrorIf(!WhisperWebsocket.IsValid(), "!WhisperWebsocket.IsValid()")) return;

  const FString Message = MakeJsonString("message", "start_capture", "language", GetLanguageAsIsoString());
  WhisperWebsocket->Send(Message);
  // if (bDebug)  GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green, TEXT("EnableAudioCapture"));

  bAudioCaptureBelowMinDuration = false;
  AudioCaptureTimer = GetWorld()->GetTimeSeconds();
}

void AAvatarsPlayerController::DisableAudioCapture()
{
  if (ULog::ErrorIf(!WhisperWebsocket.IsValid(), "!WhisperWebsocket.IsValid()")) return;

  WhisperWebsocket->Send("stop_capture");
  // if (bDebug) GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green, TEXT("DisableAudioCapture"));

  bAudioCaptureBelowMinDuration = GetWorld()->GetTimeSeconds() - AudioCaptureTimer < MinAudioCaptureDuration;
  if (bAudioCaptureBelowMinDuration)
  {
    if (AAvatarPawn* Avatar = GetSelectedAvatar())
    {
      Avatar->SetState(EAvatarState::Idle);
    }
    const FString InfoMessage = "Audio capture duration is below the minimum duration.";
    UE_LOG(LogAwatarsPlayerController, Display, TEXT("%s"), *InfoMessage);
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, InfoMessage);
  }
}

void AAvatarsPlayerController::SetLanguage(EAvatarLanguage InLanguage)
{
  this->DisableInput(this);
  if (RootWidget != nullptr)
  {
    RootWidget->OnLanguageSwitch(InLanguage);
  }

  Language = InLanguage;
  /* When changing the avatar make sure to set conversation to -1 and start a new one. */
  // ConversationId = -1;
  // ConversationId_v2 = "";
  StopDialog();

  if (OnLanguageChangeEvent.IsBound())
  {
    OnLanguageChangeEvent.Broadcast(Language);
  }

  FTimerHandle Handle;
  auto HideLoadingScreen = FTimerDelegate::CreateLambda([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)] {
    if (ULog::ErrorIf(!WeakController.IsValid(), "AvatarsPlayerController this reference is not valid HideLoadingScreen lambda in AAvatarsPlayerController.")) return;

    WeakController->EnableInput(WeakController.Get());
    WeakController->RootWidget->HideLoadingScreen();
    WeakController->StartNewConversation(true);
  });
  GetWorldTimerManager().SetTimer(Handle, HideLoadingScreen, InitialLoadDelay, false);
}

void AAvatarsPlayerController::SetCameraZoomByAlpha(const float Alpha)
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return;

  Avatar->SetCameraZoomByAlpha(Alpha);
}

void AAvatarsPlayerController::OnCameraZoomInput(const float Zoom)
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return;

  Avatar->OnCameraZoomInput(-Zoom * CameraZoomSpeed * GetWorld()->GetDeltaSeconds());
}

void AAvatarsPlayerController::OnCameraMoveInput(const FVector2D& Movement)
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return;

  Avatar->OnCameraMoveInput(Movement);
}

void AAvatarsPlayerController::ResetCameraLocation()
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return;

  Avatar->ResetCameraLocation();
}

FString AAvatarsPlayerController::GetLanguageAsApiString()
{
  switch (Language)
  {
  case EAvatarLanguage::PL:
    return TEXT("pol");
  case EAvatarLanguage::EN:
    return TEXT("eng");
  default:
    checkNoEntry();
    return TEXT("");
  }
}

FString AAvatarsPlayerController::GetLanguageAsLocalString()
{
  switch (Language)
  {
  case EAvatarLanguage::PL:
    return TEXT("PL");
  case EAvatarLanguage::EN:
    return TEXT("EN");
  default:
    checkNoEntry();
    return TEXT("");
  }
}

FString AAvatarsPlayerController::GetLanguageAsIsoString()
{
  switch (Language)
  {
  case EAvatarLanguage::PL:
    return TEXT("pl");
  case EAvatarLanguage::EN:
    return TEXT("en");
  default:
    checkNoEntry();
    return TEXT("");
  }
}

void AAvatarsPlayerController::StopDialog()
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return;

  Avatar->StopDialog();
}

void AAvatarsPlayerController::DisplayAvatarMessage(FString Message, FString AudioPath, const TArray<FString>& NewSuggestions, const TArray<FString>& ResponseTags)
{
  const bool bLongMessage = ResponseTags.Contains(TEXT("long_message"));
  if (bLongMessage)
  {
    if (bDebug)
    {
      FString DebugMessage(TEXT("Includes \"Long message\" tag."));
      UE_LOG(LogAwatarsPlayerController, Display, TEXT("%s"), *DebugMessage);
      // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, DebugMessage);
    }
    UE_LOG(LogAwatarsPlayerController, Display, TEXT("%s"), *Message);
  }
  // if (ULog::ErrorIf(!bShowAvatarMessage, "Tried to display avatar message but the bShowAvatarMessage flag is false")) return;
  Suggestions = NewSuggestions;

  AudioPath = AudioPath.Replace(TEXT(".wav"), TEXT(""));
  // verify(RootWidget != nullptr);

  if (bShowAvatarMessage)
  {
    RootWidget->ShowAvatarMessage(Message);
  }
  else
  {
    RootWidget->HideAvatarMessage(HideUserMessageDelay);
  }

  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return;

  if (RootWidget != nullptr)
  {
    RootWidget->HideSuggestionsText();
  }

  Avatar->PlayDialog(Avatar->AvatarData.AssetsPath, AudioPath, GetLanguageAsLocalString(), bLongMessage);
  RootWidget->HideStatusMessage();
}

void AAvatarsPlayerController::OnDialogStopped()
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr || RootWidget == nullptr) return;

  if (Suggestions.Num() > 0 && Avatar->GetState() == EAvatarState::Idle)
  {
    RootWidget->SetSuggestionsText(Suggestions);
    RootWidget->ShowSuggestionsText();
  }
}

void AAvatarsPlayerController::OnCharacterSelection(AActor* SelectedActor)
{
  if (SelectedActor == nullptr)
  {
    FString Message = "SelectedActor is nullptr in AAvatarsPlayerController::OnCharacterSelection";
    UE_LOG(LogAwatarsPlayerController, Error, TEXT("%s"), *Message);
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, Message);
  }

  AAvatarPawn* SelectedAvatar = Cast<AAvatarPawn>(SelectedActor);
  if (SelectedAvatar == nullptr)
  {
    FString Message = "SelectedAvatar is nullptr in AAvatarsPlayerController::OnCharacterSelection";
    UE_LOG(LogAwatarsPlayerController, Error, TEXT("%s"), *Message);
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, Message);
  }

  // ! When changing the avatar make sure to set conversation to -1 and start a new one.

  AAvatarPawn* PreviousAvatar = GetSelectedAvatar();
  if (PreviousAvatar == SelectedAvatar)
  {
    FString Message = "Selected the same avatar as the current one.";
    UE_LOG(LogAwatarsPlayerController, Warning, TEXT("%s"), *Message);
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, Message);
    return;
  }

  if (PreviousAvatar != nullptr)
  {
    SelectedAvatar->IntellectComponent->OnAiResponseEvent.RemoveDynamic(this, &AAvatarsPlayerController::DisplayAvatarMessage);
    PreviousAvatar->AudioComponent->OnAudioFinished.RemoveDynamic(this, &AAvatarsPlayerController::OnDialogStopped);
    PreviousAvatar->OnAvatarStateChangeEvent.RemoveDynamic(this, &AAvatarsPlayerController::OnAvatarStateChanged);
    PreviousAvatar->StopDialog();
    PreviousAvatar->StopAudioStream();
    UnPossess();
  }

  if (RootWidget != nullptr)
  {
    RootWidget->HideSuggestionsText();
  }

  if (bDebug)
  {
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, *("Selected avatar: " + SelectedAvatar->AvatarData.Name));
  }

  Possess(SelectedAvatar);
  SelectedAvatar->IntellectComponent->OnAiResponseEvent.AddDynamic(this, &AAvatarsPlayerController::DisplayAvatarMessage);
  // Just in case make sure that these handlers are not bound
  SelectedAvatar->AudioComponent->OnAudioFinished.RemoveDynamic(this, &AAvatarsPlayerController::OnDialogStopped);
  SelectedAvatar->OnAvatarStateChangeEvent.RemoveDynamic(this, &AAvatarsPlayerController::OnAvatarStateChanged);

  SelectedAvatar->AudioComponent->OnAudioFinished.AddDynamic(this, &AAvatarsPlayerController::OnDialogStopped);
  SelectedAvatar->OnAvatarStateChangeEvent.AddDynamic(this, &AAvatarsPlayerController::OnAvatarStateChanged);
  SelectedAvatar->SetState(EAvatarState::Idle);
  if (bUseLocalTranscription) SelectedAvatar->StartAudioStream();
  SelectedAvatar->bDebug = bDebug;

  const bool bHasCustomCamera = SelectedAvatar->CheckCustomCamera(this);
  if (!bHasCustomCamera && bDebug)
  {
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, *("Selected avatar: " + SelectedAvatar->AvatarData.Name + " does not specify its own camera"));
  }

  if (UPersistanceController* Persistance = UAvatarsGame::GetPersistance(GetWorld()))
  {
    // ! check persistance across all app!! some settings are not saved or not retrieved
    FAvatarSettings& Settings = Persistance->GetAvatarSettigns(SelectedAvatar->AvatarData.Name);
    SelectedAvatar->ApplySettings(Settings);
  }

  if (RootWidget != nullptr)
  {
    RootWidget->SelectThumbnail(SelectedAvatar);
  }

  ChangeAvatarTimeout.ClearIfValid(GetWorld());
  if (ChangeAvatarTimeout.bUseTimeout)
  {
    StartChangeAvatarTimeout();
  }

  IdleGreetingTimeout.ClearIfValid(GetWorld());
  if (IdleGreetingTimeout.bUseTimeout)
  {
    StartGreetingTimeout();
  }

  if (bPlayInitialDialog)
  {
    FTimerHandle Handle;
    GetWorldTimerManager().SetTimer(
        Handle,
        FTimerDelegate::CreateLambda([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)] {
          if (WeakController.IsValid())
          {
            WeakController->StartNewConversation(true);
          };
        }),
        1.0f,
        false
    );
  }
}

AAvatarPawn* AAvatarsPlayerController::GetNextAvatar()
{
  FAiIdentity AvatarData;
  AAvatarPawn* SelectedAvatar = GetSelectedAvatar();
  if (SelectedAvatar == nullptr)
  {
    checkNoEntry();
    return nullptr;
  }

  int32 Index = -1;
  int32 Count = SpawnedAvatars.Num();

  for (int32 i = 0; i < Count; i++)
  {
    AAvatarPawn* NextAvatar = SpawnedAvatars[i];
    if (NextAvatar != nullptr && NextAvatar->AvatarData.Name == SelectedAvatar->AvatarData.Name)
    {
      Index = i;
      break;
    }
  }

  if (Index == -1)
  {
    checkNoEntry();
  }

  Index = (Index + 1) % Count; // Wrap around using modulo
  return SpawnedAvatars[Index];
}

void AAvatarsPlayerController::StartChangeAvatarTimeout()
{
  if (!ChangeAvatarTimeout.IsReady())
  {
    ChangeAvatarTimeout.ClearIfValid(GetWorld());
    ULog::WarnIf(bDebug, "Tried to start Change Avatar timer with ChangeAvatarTimeout.IsReady() = false.");
    return;
  };

  ChangeAvatarTimeout.ClearIfValid(GetWorld());

  // Convert timeout from seconds to minutes
  const float TimeoutInSeconds = ChangeAvatarTimeout.TimeoutDelay * 60.0f;
  GetWorldTimerManager().SetTimer(ChangeAvatarTimeout.TimerHandle, this, &AAvatarsPlayerController::OnAvatarChangeTimeout, TimeoutInSeconds, false);
  if (bDebug) ULog::Info("Starting Change Avatar timer with timeout: " + ChangeAvatarTimeout.GetDelayAsText().ToString());
}

void AAvatarsPlayerController::OnAvatarChangeTimeout()
{
  EAvatarState CurrentState;
  if (!GetActiveAvatarState(CurrentState) || CurrentState != EAvatarState::Idle)
  {
    StartChangeAvatarTimeout();
    return;
  };

  ChangeAvatarTimeout.ClearIfValid(GetWorld());

  AAvatarPawn* NextAvatar = GetNextAvatar();
  if (NextAvatar == nullptr)
  {
    FString Message = "NextAvatar is nullptr in AAvatarsPlayerController::OnAvatarChangeTimeout";
    UE_LOG(LogAwatarsPlayerController, Error, TEXT("%s"), *Message);
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, Message);
    checkNoEntry();
    return;
  }

  OnCharacterSelection(NextAvatar);
}

void AAvatarsPlayerController::StartGreetingTimeout()
{
  if (!IdleGreetingTimeout.IsReady())
  {
    IdleGreetingTimeout.ClearIfValid(GetWorld());
    ULog::WarnIf(bDebug, "Tried to start Change Avatar timer with ChangeAvatarTimeout.IsReady() = false.");
    return;
  };

  IdleGreetingTimeout.ClearIfValid(GetWorld());

  const float TimeoutInSeconds = IdleGreetingTimeout.TimeoutDelay * 60.0f;
  GetWorldTimerManager().SetTimer(IdleGreetingTimeout.TimerHandle, this, &AAvatarsPlayerController::OnGreetingTimeout, TimeoutInSeconds, false);
}

void AAvatarsPlayerController::OnGreetingTimeout()
{
  EAvatarState CurrentState;
  if (!GetActiveAvatarState(CurrentState) || CurrentState != EAvatarState::Idle)
  {
    StartGreetingTimeout();
    return;
  };

  IdleGreetingTimeout.ClearIfValid(GetWorld());
  StartNewConversation(true);
  StartGreetingTimeout();
}

void AAvatarsPlayerController::StartThinkingTimeout()
{
  ThinkingTimeout.ClearIfValid(GetWorld());

  if (!ThinkingTimeout.IsReady())
  {
    if (bDebug)
    {
      ULog::Error("Tried to start Thinking Avatar timer with ThinkingTimeout.IsReady() = false.");
    }
    return;
  };

  ThinkingTimeout.ClearIfValid(GetWorld());

  GetWorldTimerManager().SetTimer(ThinkingTimeout.TimerHandle, this, &AAvatarsPlayerController::OnThinkingTimeout, ThinkingTimeout.TimeoutDelay, false);
}

void AAvatarsPlayerController::OnThinkingTimeout()
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return;

  if (Avatar->IsInState(EAvatarState::Thinking))
  {
    Avatar->SetState(EAvatarState::Idle);
  }
}

AAvatarPawn* AAvatarsPlayerController::GetSelectedAvatar() const
{
  APawn* MyPawn = GetPawn();
  if (ULog::WarnIf(MyPawn == nullptr, "Could not get reference to currently owned pawn."))
  {
    return nullptr;
  };

  AAvatarPawn* Avatar = Cast<AAvatarPawn>(MyPawn);
  if (ULog::WarnIf(Avatar == nullptr, "Could not cast pawn to AAvatarPawn."))
  {
    return nullptr;
  };

  return Avatar;
}

// ! switch this back to structure reference
bool AAvatarsPlayerController::GetActiveAvatarState(EAvatarState& OutState)
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (!Avatar) return false;

  OutState = Avatar->GetState();
  return true;
}

void AAvatarsPlayerController::OnAvatarStateChanged(EAvatarState OldState, EAvatarState NewState)
{
  if (NewState == EAvatarState::Thinking)
  {
    StartThinkingTimeout();
  }

  if (RootWidget == nullptr) return;

  if (NewState == EAvatarState::Idle && Suggestions.Num() > 0)
  {
    RootWidget->SetSuggestionsText(Suggestions);
    RootWidget->ShowSuggestionsText();
  }

  RootWidget->SetStateMessage(NewState);

  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (!Avatar) return;

  if (bDebug && RootWidget != nullptr && RootWidget->DebugInterface != nullptr)
  {
    RootWidget->DebugInterface->SetAvatarState(Avatar->MapStateToString(NewState));
  }
}

void AAvatarsPlayerController::SetAvatarState(EAvatarState State)
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (!Avatar) return;

  Avatar->SetState(State);

  if (RootWidget == nullptr) return;

  RootWidget->SetStateMessage(State);
}

AAvatarPawn* AAvatarsPlayerController::GetDefaultAvatar() const
{
  if (SpawnedAvatars.Num() == 0)
  {
    checkNoEntry();
    return nullptr;
  }

  return SpawnedAvatars[0];
}

bool AAvatarsPlayerController::IsExcludedPhrase(const FString& Phrase) const
{
  const FString CleanPhrase = Phrase.ToLower().TrimStartAndEnd();
  for (const FString& String : ExcludedPhrases)
  {
    const FString CleanString = String.ToLower().TrimStartAndEnd();
    if (CleanPhrase.Contains(CleanString))
    {
      return true;
    }
  }
  return false;
}

bool AAvatarsPlayerController::SendUserMessage(FString Message)
{

  if (Message.Contains("[FINAL]"))
  {
    Message.ReplaceInline(TEXT("[FINAL]"), TEXT(""), ESearchCase::IgnoreCase);
  }

  if (bDebug)
  {
    FString DebugMessage = "User past message: " + Message;
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::White, DebugMessage);
    UE_LOG(LogAwatarsPlayerController, Display, TEXT("%s"), *DebugMessage);
  }

  Message.TrimStartAndEndInline();

  if (Message == "")
  {
    // if (bDebug) GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, TEXT("User past EMPTY message"));
    return false;
  }

  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (!Avatar)
  {
    FString ErrorMessage = "Could not get reference to AAvatarPawn* Avatar in AAvatarsPlayerController::SendUserMessage";
    UE_LOG(LogAwatarsPlayerController, Error, TEXT("%s"), *ErrorMessage);
    // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Red, ErrorMessage);
    return false;
  }

  // Make sure that new message from user will reset avatar change timeout.
  ChangeAvatarTimeout.ClearIfValid(GetWorld());
  if (ChangeAvatarTimeout.bUseTimeout)
  {
    StartChangeAvatarTimeout();
  }

  // Make sure that new message from user will reset greeting timeout.
  IdleGreetingTimeout.ClearIfValid(GetWorld());
  if (IdleGreetingTimeout.bUseTimeout)
  {
    StartGreetingTimeout();
  }

  return Avatar->IntellectComponent->RespondTo(Message, GetLanguageAsIsoString());
}

void AAvatarsPlayerController::OnAnyKey()
{
  if (RootWidget != nullptr && RootWidget->OptionsMenu != nullptr)
  {
    RootWidget->OptionsMenu->StartMenuTimeout();
  }

  ChangeAvatarTimeout.ClearIfValid(GetWorld());
  if (ChangeAvatarTimeout.IsReady())
  {
    StartChangeAvatarTimeout();
  }

  IdleGreetingTimeout.ClearIfValid(GetWorld());
  if (IdleGreetingTimeout.IsReady())
  {
    StartGreetingTimeout();
  }
}
