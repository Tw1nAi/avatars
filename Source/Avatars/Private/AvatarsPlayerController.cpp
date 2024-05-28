// Copyright Juice sp. z o. o., All Rights Reserved.

#include "AvatarsPlayerController.h"

#include "Components/AudioComponent.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"

#include "AvatarPawn.h"
#include "AvatarsApi/Api_v1/AvatarsApi.h"
#include "AvatarsApi/Api_v1/Models/Actions.h"
#include "AvatarsApi/Api_v1/Models/Conversation.h"
#include "AvatarsApi/Api_v1/Models/UserMessage.h"
#include "AvatarsApi/Api_v2/AvatarsApi_v2.h"
#include "AvatarsApi/Api_v2/Models/Avatar_v2.h"
#include "AvatarsApi/Api_v2/Models/Conversation_v2.h"
#include "AvatarsApi/Api_v2/Models/UserMessage_v2.h"
#include "AvatarsGame.h"
#include "Get.h"
#include "Log.h"
#include "Persistance/PersistanceController.h"
#include "UserInterface/RootWidget.h"

AAvatarsPlayerController::AAvatarsPlayerController()
{
  ExcludedPhrases.Add(TEXT("KONIEC!"));
  IdleGreetingTimeout.TimeoutDelay = 30.0f;
  bApiDone = true;
  DefaultAvatar = EAvatarCharacter::WojtekTheBear;
}

AAvatarsPlayerController* AAvatarsPlayerController::Get(UWorld* World)
{
  if (ULog::ErrorIf(World == nullptr, "World is nullptr")) return nullptr;

  APlayerController* Controller = UGameplayStatics::GetPlayerController(World, 0);
  if (Controller == nullptr)
  {
    // ! find out why this is happening
    // ULog::Error("Could not get reference to AAvatarsPlayerController* PlayerController");
    return nullptr;
  }

  AAvatarsPlayerController* OutController = Cast<AAvatarsPlayerController>(Controller);
  if (OutController == nullptr)
  {
    // ! find out why this is happening
    // ULog::Error("Could not cast to AAvatarsPlayerController");
    return nullptr;
  }

  return OutController;
}

void AAvatarsPlayerController::BeginPlay()
{
  Super::BeginPlay();

  // TODO : move this somehow to the game instance
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
  LoadAvatarsData();
  SetupWidgets();
  SetupApi_v1();
  SetupApi_v2();

  if (bUseLocalTranscription)
  {
    // Websockets transcription is currently used only with local transcription.
    SetupWhisperWebsockets();
    ConnectToWhisperServer();
  }

  // ScheduleHideLoadingScreen();
}

void AAvatarsPlayerController::Tick(float DeltaSeconds)
{
  Super::Tick(DeltaSeconds);
}

void AAvatarsPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Super::EndPlay(EndPlayReason);

  CleanupWebSocket();
}

void AAvatarsPlayerController::LoadAvatarsData()
{
  if (AvatarsDataTable)
  {
    for (const auto& RowIt : AvatarsDataTable->GetRowMap())
    {
      FName RowName = RowIt.Key;
      FAvatarData* AvatarData = reinterpret_cast<FAvatarData*>(RowIt.Value);

      if (AvatarData == nullptr)
      {
        if (bDebug) ULog::Error("Could not get row data from AvatarsDataTable.");
        continue;
      }
      AvatarData->Id.AvatarTag = AvatarData->AvatarTag;
      Avatars.Add(*AvatarData);
      if (bDebug)
      {
        UE_LOG(LogTemp, Log, TEXT("AvatarTag: %d"), static_cast<uint8>(AvatarData->AvatarTag));
        UE_LOG(LogTemp, Log, TEXT("Name: %s"), *AvatarData->Name);
        UE_LOG(LogTemp, Log, TEXT("AssetsPath: %s"), *AvatarData->AssetsPath);
        UE_LOG(LogTemp, Log, TEXT("ApiVersion: %d"), static_cast<uint8>(AvatarData->ApiVersion));
        UE_LOG(LogTemp, Log, TEXT("AvatarClass: %s"), *AvatarData->AvatarClass->GetName());
        if (AvatarData->ThumbnailImage)
        {
          UE_LOG(LogTemp, Log, TEXT("ThumbnailImage: %s"), *AvatarData->ThumbnailImage->GetName());
        }
        else
        {
          UE_LOG(LogTemp, Log, TEXT("ThumbnailImage: None"));
        }
      }
    }
  }
  GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("message"));
}

void AAvatarsPlayerController::SetupWidgets()
{
  if (RootWidgetClass != nullptr)
  {
    UUserWidget* Widget = CreateWidget<UUserWidget>(this, RootWidgetClass);
    if (Widget != nullptr)
    {
      Widget->AddToViewport();
      RootWidget = Cast<URootWidget>(Widget);
      RootWidget->AddToViewport();
      RootWidget->OnCharacterThumbnailClickEvent.AddUObject(this, &AAvatarsPlayerController::OnCharacterSelection);
      // RootWidget->bDebug = bDebug;
    }
  }
}

void AAvatarsPlayerController::ScheduleHideLoadingScreen()
{
  FTimerHandle Handle;
  auto HideLoadingScreen = FTimerDelegate::CreateLambda([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)] {
    if (ULog::ErrorIf(
            !WeakController.IsValid(),
            "AvatarsPlayerController this reference is not valid AvatarsPlayerController BeginPlay HideLoadingScreen lambda"
        ))
      return;
    WeakController->EnableInput(WeakController.Get());
    WeakController->RootWidget->HideLoadingScreen();
  });
  GetWorldTimerManager().SetTimer(Handle, HideLoadingScreen, InitialLoadDelay, false);
}

void AAvatarsPlayerController::SetupApi_v1()
{
  Api = NewObject<UAvatarsApi>(this, UAvatarsApi::StaticClass());
  if (Api == nullptr)
  {
    if (bDebug) ULog::Error("Could not create instance of UAvatarsApi.");
    return;
  }

  Api->bDebug = bDebug;
  Api->SetBaseUrl("http://localhost:5001/api/v1");
  Api->AddDefaultHeader("Content-Type", "application/json");

  /* Create initial requests. */
  Api->GetAvatars()
      ->Then([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)](const FGetAvatarsResponse& Data) mutable {
        if (ULog::ErrorIf(
                !WeakController.IsValid(),
                "AvatarsPlayerController this reference is not valid Api->GetAvatars() lambda in AAvatarsPlayerController"
            ))
          return;

        // for (const FAvatar RemoteAvatar : Data.Avatars)
        // {
        //   const bool bIsZumbach = RemoteAvatar.Name.ToString().Contains("zumbach");
        //   FString SearchName = bIsZumbach ? "Jan Zumbach" : "Jan Kowalewski";

        //   for (FAvatarData& LocalData : WeakController->Avatars)
        //   {
        //     if (LocalData.ApiVersion == EApiVersion::API_v1 && LocalData.MatchByName == SearchName)
        //     {
        //       LocalData.Id.v1 = RemoteAvatar.Id;
        //       // // ! directory could be set in data table
        //       // FString Directory = bIsZumbach ? "JanZumbach" : "JanKowalewski";
        //       // LocalData.AssetsPath = "/Game/Avatars/Characters/" + Directory + "/";
        //       // const EAvatarCharacter Tag = bIsZumbach ? EAvatarCharacter::JanZumbach : EAvatarCharacter::JanKowalewski;
        //       // LocalData.AvatarTag = Tag;
        //       // LocalData.Id.AvatarTag = Tag;
        //     }
        //   }
        // }

        // WeakController->OnAvatarsDataReceived();
      })
      ->Run();
}

void AAvatarsPlayerController::SetupApi_v2()
{
  Api_v2 = NewObject<UAvatarsApi_v2>(this, UAvatarsApi_v2::StaticClass());
  if (Api_v2 == nullptr)
  {
    if (bDebug) ULog::Error("Could not create instance of UAvatarsApi_v2.");
    return;
  }

  Api_v2->bDebug = bDebug;
  Api_v2->SetBaseUrl("http://localhost:5005/api/v2");
  Api_v2->AddDefaultHeader("Content-Type", "application/json");

  /* Create initial requests. */
  Api_v2->GetAvatars()
      ->Then([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)](const FGetAvatarsResponse_v2& Data) mutable {
        if (ULog::ErrorIf(
                !WeakController.IsValid(),
                "AvatarsPlayerController this reference is not valid in Api_v2->GetAvatars() in AAvatarsPlayerController"
            ))
          return;
        if (ULog::ErrorIf(Data.Avatars.Num() == 0, "No avatars data received from Api_v2")) return;

        for (const FAvatar_v2 RemoteAvatar : Data.Avatars)
        {
          for (FAvatarData& LocalData : WeakController->Avatars)
          {
            if (LocalData.ApiVersion == EApiVersion::API_v2 && LocalData.MatchByName(RemoteAvatar.AvatarName))
            {
              LocalData.Id.v2 = RemoteAvatar.AvatarId;
              LocalData.Name = RemoteAvatar.AvatarName;
            }
          }
        }

        WeakController->OnAvatarsDataReceived();
      })
      ->Run();
}

void AAvatarsPlayerController::SetupWhisperWebsockets()
{
  FString Url = WhisperWebsocketServerProtocol + "://" + WhisperWebsocketServerURL + ":" + WhisperWebsocketPort + "/";

  if (bDebug) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, Url);

  WhisperWebsocket = FWebSocketsModule::Get().CreateWebSocket(Url, WhisperWebsocketServerProtocol);

  /* On Connected */
  WhisperWebsocket->OnConnected().AddLambda([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)]() mutable {
    if (ULog::ErrorIf(
            !WeakController.IsValid(), "AvatarsPlayerController this reference is not valid in WhisperWebsocket->OnConnected lambda"
        ))
      return;

    ULog::InfoIf(WeakController->bDebug, "Connected to WhisperWebsocket");
    WeakController->WhisperWebsocket->Send(WeakController->OnConnectedResponse);
  });

  /* On Error */
  WhisperWebsocket->OnConnectionError().AddLambda([](const FString& Error) mutable { ULog::Error("Websockets connection error: " + Error); }
  );

  /* On Closed */
  WhisperWebsocket->OnClosed().AddLambda([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this
                                          )](int32 StatusCode, const FString& Reason, bool bWasClean) mutable {
    if (ULog::ErrorIf(
            !WeakController.IsValid(), "AvatarsPlayerController this reference is not valid in WhisperWebsocket->OnClosed lambda"
        ))
      return;

    ULog::InfoIf(
        WeakController->bDebug,
        "Closed WhisperWebsocket connection. Reason: " + Reason + " StatusCode: " + FString::FromInt(StatusCode) +
            " bWasClean: " + (bWasClean ? "true" : "false")
    );
  });

  /* On Message */
  WhisperWebsocket->OnMessage().AddLambda([WeakController =
                                               TWeakObjectPtr<AAvatarsPlayerController>(this)](const FString& Message) mutable {
    if (ULog::ErrorIf(
            !WeakController.IsValid(), "AvatarsPlayerController this reference is not valid in WhisperWebsocket->OnMessage lambda."
        ))
      return;

    if (WeakController->bDebug)
    {
      GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Green, Message);
    }

    if (!WeakController->bSendOnlyOnFinal || Message.Contains("[FINAL]"))
    {
      WeakController->ThinkingTimeout.ClearIfValid(WeakController->GetWorld());

      const bool bWasSent = WeakController->SendUserMessage(Message);
      AAvatarPawn* Avatar = WeakController->GetSelectedAvatar();
      if (Avatar != nullptr)
      {
        Avatar->SetState(bWasSent ? EAvatarState::Thinking : EAvatarState::Idle);
      }
    }

    if (WeakController->bShowUserMessage && WeakController->RootWidget != nullptr)
    {
      WeakController->RootWidget->ShowUserMessage(Message);
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
  else
  {
    // GetWorldTimerManager().PauseTimer(this->ReconnectTimerHandle);
    return "WhisperWebsocket is connected.";
  }
}

void AAvatarsPlayerController::ClearSpeachToText()
{
  if (WhisperWebsocket.IsValid())
  {
    WhisperWebsocket->Send("clear");
  }
}

void AAvatarsPlayerController::EnableAudioCapture()
{
  if (ULog::ErrorIf(!WhisperWebsocket.IsValid(), "!WhisperWebsocket.IsValid()")) return;

  if (bDebug) GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, TEXT("EnableAudioCapture"));
  WhisperWebsocket->Send("start_capture");
}

void AAvatarsPlayerController::DisableAudioCapture()
{
  if (ULog::ErrorIf(!WhisperWebsocket.IsValid(), "!WhisperWebsocket.IsValid()")) return;

  if (bDebug) GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Yellow, TEXT("DisableAudioCapture"));
  WhisperWebsocket->Send("stop_capture");
}

void AAvatarsPlayerController::SetLanguage(EAvatarLanguage InLanguage)
{
  this->DisableInput(this);
  if (RootWidget != nullptr)
  {
    RootWidget->ShowLoadingScreen();
    RootWidget->HideUserTextBox();
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
    if (ULog::ErrorIf(
            !WeakController.IsValid(),
            "AvatarsPlayerController this reference is not valid HideLoadingScreen lambda in AAvatarsPlayerController."
        ))
      return;

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

AAvatarPawn* AAvatarsPlayerController::SpawnAvatar(TSubclassOf<AAvatarPawn> AvatarClass)
{
  FVector Location;
  FRotator Rotation;

  FActorSpawnParameters SpawnParams;
  SpawnParams.Owner = this;

  AAvatarPawn* SpawnedAvatar = GetWorld()->SpawnActor<AAvatarPawn>(AvatarClass, Location, Rotation, SpawnParams);

  if (SpawnedAvatar != nullptr)
  {
    return SpawnedAvatar;
  }

  checkNoEntry();
  return nullptr;
}

void AAvatarsPlayerController::StartNewConversation(const bool bDisplayAvatarResponse)
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return;

  if (Avatar->ApiVersion == EApiVersion::API_v2)
  {
    auto PostNewConversationHandle = Api_v2->PostNewConversation(GetLanguageAsIsoString(), Avatar->Id.v2);

    PostNewConversationHandle
        ->Then([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this),
                bDisplayAvatarResponse](const FPostConversationResponse_v2& Data) mutable {
          if (ULog::ErrorIf(
                  !WeakController.IsValid(),
                  "AvatarsPlayerController this reference is not valid in PostNewConversationHandle lambda in AAvatarsPlayerController"
              ))
            return;

          // if (WeakController->bDebug) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, Data.ConversationId);
          WeakController->SetConversationId_v2(Data.ConversationId);

          if (bDisplayAvatarResponse)
          {
            WeakController->DisplayAvatarMessage_v2(Data.Message, Data.Propositions);
            return;
          }

          if (WeakController->PendingMessages.Num() > 0)
          {
            WeakController->SendUserMessage(WeakController->PendingMessages[0]);
            WeakController->PendingMessages.Empty();
          }
        })
        ->Run();

    return;
  }

  FString Lan = GetLanguageAsApiString();
  auto PostNewConversationHandle = Api->PostNewConversation(Lan, Avatar->Id.v1);

  PostNewConversationHandle
      ->Then([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this),
              bDisplayAvatarResponse](const FPostConversationResponse& Data) mutable {
        if (ULog::ErrorIf(
                !WeakController.IsValid(),
                "AvatarsPlayerController this reference is not valid in PostNewConversationHandle lambda in AAvatarsPlayerController"
            ))
          return;

        WeakController->SetConversationId(Data.Conversation.Id);
        if (Data.Conversation.Messages.Num() == 0)
        {
          // verify(false);
          return;
        }

        if (bDisplayAvatarResponse)
        {
          WeakController->DisplayAvatarMessage(Data.Conversation.Messages[0]);
          return;
        }

        if (WeakController->PendingMessages.Num() > 0)
        {
          WeakController->SendUserMessage(WeakController->PendingMessages[0]);
          WeakController->PendingMessages.Empty();
        }
      })
      ->Run();
}

bool AAvatarsPlayerController::StartedConversation()
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return false;

  return Avatar->ApiVersion == EApiVersion::API_v1 && ConversationId != -1 ||
         Avatar->ApiVersion == EApiVersion::API_v2 && ConversationId_v2 != "";
}

void AAvatarsPlayerController::DisplayAvatarMessage(FAvatarMessage Message)
{
  // if (ULog::ErrorIf(!bShowAvatarMessage, "Tried to display avatar message but the bShowAvatarMessage flag is false")) return;

  Message.AudioPath = Message.AudioPath.Replace(TEXT(".wav"), TEXT(""));
  // verify(RootWidget != nullptr);

  if (bShowAvatarMessage)
  {
    RootWidget->ShowAvatarMessage(Message.Text);
  }
  else
  {
    RootWidget->HideAvatarMessage(HideUserMessageDelay);
  }

  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr) return;

  FString AvatarAssetsPath = GetSelectedAvatarData().AssetsPath;
  Avatar->PlayDialog(AvatarAssetsPath, Message.AudioPath, GetLanguageAsLocalString());

  RootWidget->ShowPressToTalkMessage();
}

void AAvatarsPlayerController::DisplayAvatarMessage_v2(FMessage_v2 Message, const TArray<FString>& NewSuggestions)
{
  // if (ULog::ErrorIf(!bShowAvatarMessage, "Tried to display avatar message but the bShowAvatarMessage flag is false")) return;
  Suggestions = NewSuggestions;

  Message.AudioPath = Message.AudioPath.Replace(TEXT(".wav"), TEXT(""));
  // verify(RootWidget != nullptr);

  if (bShowAvatarMessage)
  {
    RootWidget->ShowAvatarMessage(Message.Text);
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
  FString AvatarAssetsPath = GetSelectedAvatarData().AssetsPath;
  Avatar->PlayDialog(AvatarAssetsPath, Message.AudioPath, GetLanguageAsLocalString());

  RootWidget->HideStatusMessage();
}

void AAvatarsPlayerController::OnDialogStopped()
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr || RootWidget == nullptr) return;

  if (Suggestions.Num() > 0 && Avatar->ApiVersion == EApiVersion::API_v2 && Avatar->GetState() == EAvatarState::Idle)
  {
    RootWidget->SetSuggestionsText(Suggestions);
    RootWidget->ShowSuggestionsText();
  }
}

void AAvatarsPlayerController::OnCharacterSelection(const FAvatarId& AvatarId)
{
  if (RootWidget != nullptr)
  {
    RootWidget->HideSuggestionsText();
  }

  /* When changing the avatar make sure to set conversation to -1 and start a new one. */
  ConversationId = -1;
  ConversationId_v2 = "";

  FAvatarData AvatarData;
  for (FAvatarData& Data : Avatars)
  {
    if (Data.Id == AvatarId)
    {
      AvatarData = Data;
      break;
    }
  }

  AAvatarPawn* SelectedAvatar = nullptr;

  TArray<AActor*> SpawnedAvatars;
  UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAvatarPawn::StaticClass(), SpawnedAvatars);

  for (AActor* SpawnedAvatar : SpawnedAvatars)
  {
    if (AAvatarPawn* SpawnedAvatarPawn = Cast<AAvatarPawn>(SpawnedAvatar))
    {
      if (SpawnedAvatarPawn->Id.AvatarTag == AvatarData.AvatarTag)
      {
        SelectedAvatar = SpawnedAvatarPawn;
        break;
      }
    }
  }

  // if (SelectedAvatar != nullptr && SelectedAvatar->Id == AvatarId)
  // {

  //   ULog::Warn(TEXT("Selected avatar: " + AvatarData.Name + " found on the map, using existing actor."));
  //   return;
  // }

  AAvatarPawn* PreviousAvatar = GetSelectedAvatar();
  if (PreviousAvatar != nullptr)
  {
    PreviousAvatar->AudioComponent->OnAudioFinished.RemoveDynamic(this, &AAvatarsPlayerController::OnDialogStopped);
    PreviousAvatar->OnAvatarStateChangeEvent.RemoveDynamic(this, &AAvatarsPlayerController::OnAvatarStateChanged);
    PreviousAvatar->StopDialog();
    PreviousAvatar->StopAudioStream();
    UnPossess();
  }

  if (SelectedAvatar == nullptr)
  {
    SelectedAvatar = SpawnAvatar(AvatarData.AvatarClass);
    if (bDebug)
    {
      GEngine->AddOnScreenDebugMessage(
          -1, 10.f, FColor::Yellow, *("Selected avatar: " + AvatarData.Name + " not found on the map, spawning new actor.")
      );
    }
  }

  SelectedAvatar->ApiVersion = AvatarData.ApiVersion;
  SelectedAvatar->Id = AvatarData.Id;
  if (bDebug)
  {
    GEngine->AddOnScreenDebugMessage(
        -1,
        10.f,
        FColor::Yellow,
        *("Selected avatar: " + AvatarData.Name + ", API version: " + (AvatarData.ApiVersion == EApiVersion::API_v2 ? "API v2" : "API v1"))
    );
  }

  // Just in case make sure that these handlers are not bound
  SelectedAvatar->AudioComponent->OnAudioFinished.RemoveDynamic(this, &AAvatarsPlayerController::OnDialogStopped);
  SelectedAvatar->OnAvatarStateChangeEvent.RemoveDynamic(this, &AAvatarsPlayerController::OnAvatarStateChanged);

  SelectedAvatar->AudioComponent->OnAudioFinished.AddDynamic(this, &AAvatarsPlayerController::OnDialogStopped);
  SelectedAvatar->OnAvatarStateChangeEvent.AddDynamic(this, &AAvatarsPlayerController::OnAvatarStateChanged);
  SelectedAvatar->SetState(EAvatarState::Idle);
  if (bUseLocalTranscription) SelectedAvatar->StartAudioStream();
  SelectedAvatar->bDebug = bDebug;

  if (UPersistanceController* Persistance = UAvatarsGame::GetPersistance(GetWorld()))
  {
    FAvatarSettings& Settings = Persistance->GetAvatarSettigns(SelectedAvatar->Id.v2);
    SelectedAvatar->ApplySettings(Settings);
  }
  Possess(SelectedAvatar);

  const bool bHasCustomCamera = SelectedAvatar->CheckCustomCamera(this);
  if (!bHasCustomCamera && bDebug)
  {
    GEngine->AddOnScreenDebugMessage(
        -1, 10.f, FColor::Yellow, *("Selected avatar: " + AvatarData.Name + " does not specify its own camera")
    );
  }

  if (RootWidget != nullptr)
  {
    RootWidget->SelectThumbnail(SelectedAvatar->Id);
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

FAvatarData AAvatarsPlayerController::GetNextAvatar()
{
  FAvatarData AvatarData;
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (Avatar == nullptr)
  {
    checkNoEntry();
    return AvatarData;
  }

  int32 Index = -1;
  int32 Count = Avatars.Num();

  for (int32 i = 0; i < Count; i++)
  {
    FAvatarData& Data = Avatars[i];
    if (Data.Id == Avatar->Id)
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
  return Avatars[Index];
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
  GetWorldTimerManager().SetTimer(
      ChangeAvatarTimeout.TimerHandle, this, &AAvatarsPlayerController::OnAvatarChangeTimeout, TimeoutInSeconds, false
  );
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
  FAvatarData Data = GetNextAvatar();
  OnCharacterSelection(Data.Id);
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
  GetWorldTimerManager().SetTimer(
      IdleGreetingTimeout.TimerHandle, this, &AAvatarsPlayerController::OnGreetingTimeout, TimeoutInSeconds, false
  );
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
  if (!ThinkingTimeout.IsReady())
  {
    ThinkingTimeout.ClearIfValid(GetWorld());
    if (bDebug)
    {
      ULog::Error("Tried to start Thinking Avatar timer with ThinkingTimeout.IsReady() = false.");
    }
    return;
  };

  ThinkingTimeout.ClearIfValid(GetWorld());

  GetWorldTimerManager().SetTimer(
      ThinkingTimeout.TimerHandle, this, &AAvatarsPlayerController::OnThinkingTimeout, ThinkingTimeout.TimeoutDelay, false
  );
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

const FAvatarId& AAvatarsPlayerController::GetSelectedAvatarId() const
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  return Avatar->Id;
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

  RootWidget->SetStateMessage(NewState);

  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (!Avatar) return;

  if (bDebug && RootWidget != nullptr && RootWidget->DebugInterface != nullptr)
  {
    RootWidget->DebugInterface->SetAvatarState(Avatar->MapStateToString(NewState));
  }
}

FAvatarData& AAvatarsPlayerController::GetSelectedAvatarData()
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (!Avatar) return Avatars[0];

  for (FAvatarData& AvatarData : Avatars)
  {
    if (AvatarData.Id == Avatar->Id) return AvatarData;
  }

  checkNoEntry();
  return Avatars[0];
}

void AAvatarsPlayerController::SetAvatarState(EAvatarState State)
{
  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (!Avatar) return;

  Avatar->SetState(State);

  if (RootWidget == nullptr) return;

  RootWidget->SetStateMessage(State);
}

const FAvatarData& AAvatarsPlayerController::GetDefaultAvatarData() const
{
  for (const FAvatarData& Data : Avatars)
  {
    if (Data.AvatarTag == DefaultAvatar) return Data;
  }

  checkNoEntry();
  static FAvatarData EmptyAvatarData;
  return EmptyAvatarData;
}

void AAvatarsPlayerController::OnAvatarsDataReceived()
{
  if (!this->bApiDone)
  {
    this->bApiDone = true;
    return;
  }

  if (ULog::ErrorIf(Avatars.Num() == 0, "No avatars data received")) return;

  if (RootWidget != nullptr)
  {
    RootWidget->CreateAvatarsThumbnails(Avatars);
  }

  FAvatarData Default = GetDefaultAvatarData();
  OnCharacterSelection(Default.Id);
  ScheduleHideLoadingScreen();
}

void AAvatarsPlayerController::SetConversationId(int32 Id)
{
  ConversationId = Id;
}

void AAvatarsPlayerController::SetConversationId_v2(FString Id)
{
  ConversationId_v2 = Id;
}

bool AAvatarsPlayerController::IsExcludedPhrase(const FString Phrase)
{
  for (const FString& String : ExcludedPhrases)
  {
    if (String.TrimStartAndEnd().Equals(Phrase.TrimStartAndEnd(), ESearchCase::IgnoreCase)) // Case insensitive match
    {
      return true;
    }
  }
  return false;
}

bool AAvatarsPlayerController::SendUserMessage(FString Message)
{
  if (bDebug) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("User message to send: " + Message));

  if (Message.Contains("[FINAL]"))
  {
    Message.ReplaceInline(TEXT("[FINAL]"), TEXT(""), ESearchCase::IgnoreCase);
  }

  if (IsExcludedPhrase(Message))
  {
    if (bDebug)
    {
      ULog::Error("Detected excluded phrase in user message: " + Message);
    }
    return false;
  }

  Message.TrimStartAndEndInline();

  if (Message == "")
  {
    if (bDebug) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("User past EMPTY message"));
    return false;
  }

  AAvatarPawn* Avatar = GetSelectedAvatar();
  if (!Avatar) return false;

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

  // The sole reason for saving a message as pending is to avoid sending it before the conversation is started.
  // There should be no other reason to save a message as pending.
  if (Avatar->ApiVersion == EApiVersion::API_v1 && ConversationId == -1 ||
      Avatar->ApiVersion == EApiVersion::API_v2 && ConversationId_v2 == "")
  {

    if (bDebug)
    {
      GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::SanitizeFloat((float)ConversationId));
      GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, *ConversationId_v2);
    }

    PendingMessages.Add(Message);
    StartNewConversation(Avatar->ApiVersion == EApiVersion::API_v2);
    return false;
  }

  if (Avatar->ApiVersion == EApiVersion::API_v2)
  {
    if (ULog::ErrorIf(Api_v2 == nullptr, "Api_v2 is nullptr")) return false;

    Api_v2->PostUserMessage(Message, ConversationId_v2)
        ->Then([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)](const FPostUserMessageResponse_v2& Response) {
          if (ULog::ErrorIf(
                  !WeakController.IsValid(),
                  "AvatarsPlayerController this reference is not valid in AAvatarsPlayerController::SendUserMessage > "
                  "Api->PostUserMessage "
                  "lambda"
              ))
            return;

          WeakController->DisplayAvatarMessage_v2(Response.Message, Response.Propositions);
        })
        ->Run();

    return true;
  }

  if (ULog::ErrorIf(Api == nullptr, "Api is nullptr")) return false;
  Api->PostUserMessage(Message, ConversationId)
      ->Then([WeakController = TWeakObjectPtr<AAvatarsPlayerController>(this)](const FPostUserMessageResponse& Response) {
        if (ULog::ErrorIf(
                !WeakController.IsValid(),
                "AvatarsPlayerController this reference is not valid in AAvatarsPlayerController::SendUserMessage > Api->PostUserMessage "
                "lambda"
            ))
          return;

        WeakController->DisplayAvatarMessage(Response.Message);
      })
      ->Run();

  return true;
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
