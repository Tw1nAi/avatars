// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/RootWidget.h"

#include "Components/GridSlot.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Slate/SceneViewport.h"

#include "AiIdentityInterface.h"
#include "AvatarsPlayerController.h"
#include "Get.h"
#include "Log.h"
#include "Translations.h"
#include "UserInterface/LoadingScreenWidget.h"

void URootWidget::NativeConstruct()
{
  Super::NativeConstruct();

  if (SendTextButton != nullptr)
  {
    SendTextButton->OnClicked.AddDynamic(this, &URootWidget::OnSendTextButtonClick);
  }

  HiddenWidgets.Add(StatusMessge);
  HiddenWidgets.Add(UserTextInputBox);
  HiddenWidgets.Add(OptionsMenu);
  HiddenWidgets.Add(UserMessage);
  HiddenWidgets.Add(AvatarMessage);
  HiddenWidgets.Add(Suggestions);

  for (UWidget* Widget : HiddenWidgets)
  {
    if (Widget != nullptr) Widget->SetVisibility(ESlateVisibility::Collapsed);
  }

  if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
  {
    // Bind to the viewport resize event
    ViewportResizedHandle = GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &URootWidget::OnViewportResized);

    // Initialize the last viewport size
    FViewport* Viewport = GEngine->GameViewport->Viewport;
    if (Viewport)
    {
      LastViewportSize = FVector2D(Viewport->GetSizeXY());
      FString Message = FString::Printf(TEXT("Viewport size initialized to: %s"), *LastViewportSize.ToString());
      // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, Message);
      UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
    }
  }
}

void URootWidget::NativeDestruct()
{
  if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
  {
    GEngine->GameViewport->Viewport->ViewportResizedEvent.Remove(ViewportResizedHandle);
  }

  Super::NativeDestruct();
}

void URootWidget::SynchronizeProperties()
{
  Super::SynchronizeProperties();
  SetupDebugHelpers();
}

bool URootWidget::GetController()
{
  PlayerController = AAvatarsPlayerController::Get(GetWorld());
  return PlayerController != nullptr;
}

void URootWidget::SetupDebugHelpers()
{
  if (OptionsMenu != nullptr)
  {
    if (DebugOpenOptionsButton != nullptr)
    {
      if (bDebug)
      {
        OptionsMenu->SetUseTimeout(false);
        DebugOpenOptionsButton->OnClicked.Clear();
        DebugOpenOptionsButton->OnClicked.AddDynamic(OptionsMenu, &UOptionsMenu::Show);
        DebugOpenOptionsButton->SetVisibility(ESlateVisibility::Visible);
      }
      else
      {
        DebugOpenOptionsButton->SetVisibility(ESlateVisibility::Collapsed);
      }
    }
    if (DebugInterface != nullptr)
    {
      DebugInterface->SetVisibility(bDebug ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
  }
}

void URootWidget::ShowSuggestionsText()
{
  if (SuggestionsText == nullptr)
  {
    checkNoEntry();
    ULog::Error("SuggestionsText == nullptr.");
    return;
  }

  HideUserMessage();
  HideAvatarMessage();
  Suggestions->SetVisibility(ESlateVisibility::Visible);
}

void URootWidget::HideSuggestionsText()
{
  if (Suggestions != nullptr)
  {
    Suggestions->SetVisibility(ESlateVisibility::Collapsed);
  }
}

void URootWidget::SetSuggestionsText(const TArray<FString>& NewSuggestions)
{
  if (SuggestionsText != nullptr)
  {
    TArray<FString> FormattedSuggestions;
    for (FString Suggestion : NewSuggestions)
    {
      FormattedSuggestions.Add(TEXT("• ") + Suggestion);
    }
    SuggestionsText->SetText(FText::FromString(FString::Join(FormattedSuggestions, TEXT("\n"))));
  }
}

// ! this should be moved to PlayerController/GameInstance
void URootWidget::OnSendTextButtonClick()
{
  if (!GetController())
  {
    HideUserTextBox();
    return;
  }

  PlayerController->ThinkingTimeout.ClearIfValid(GetWorld());

  const FString Message = UserTextBox->GetText().ToString();
  if (Message.IsEmpty())
  {
    HideUserTextBox();
    return;
  }

  const bool bIsCommand = (Message[0] == TEXT('/') || Message[0] == TEXT('\\')) && Message.Len() > 1;
  if (bIsCommand)
  {
    FString Command = Message.RightChop(1);
    // if (bDebug)  GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("OnSendTextButtonClick: found command: ") + Command);

    if (Command.Contains(PlayerController->ServicePin) && Command.Contains("options"))
    {
      if (bDebug)
      {
        Command = Command.Replace(*PlayerController->ServicePin, TEXT("")).TrimStartAndEnd();
        // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, (TEXT("OnSendTextButtonClick: executing command: ") + Command));
      }
      if (OptionsMenu != nullptr)
      {
        OptionsMenu->Show();
        OptionsMenu->SetUseTimeout(true);
        OptionsMenu->StartMenuTimeout(PlayerController->MenuTimeout);
      }
    };
  }
  else
  {
    UserTextBox->SetText(FText());
    PlayerController->SendUserMessage(Message);
  }

  HideUserTextBox();
}

void URootWidget::HideUserTextBox()
{
  UserTextInputBox->SetVisibility(ESlateVisibility::Collapsed);
  UserTextBox->SetText(FText());
}

void URootWidget::ShowUserTextBox()
{
  if (GetController())
  {
    PlayerController->StopDialog();
    PlayerController->SetAvatarState(EAvatarState::Listening);
  }

  StatusMessge->SetVisibility(ESlateVisibility::Collapsed);
  UserMessage->SetVisibility(ESlateVisibility::Collapsed);
  HideSuggestionsText();

  UserTextInputBox->SetVisibility(ESlateVisibility::Visible);
  UserTextBox->SetKeyboardFocus();
}

void URootWidget::ShowUserMessage(FString Message, const float HideDelay)
{
  verify(UserMessage != nullptr);
  if (UserMessage == nullptr) return;

  HideSuggestionsText();
  Message.TrimStartAndEndInline();
  UserMessage->SetText(FText::FromString(Message.Left(1).ToUpper() + Message.RightChop(1)));
  UserMessage->SetVisibility(ESlateVisibility::Visible);

  if (HideDelay > 0.0f)
  {
    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] { this->HideUserMessage(0.0); }), HideDelay, false);
  }
}

void URootWidget::StartNewConversation()
{
  if (GetController())
  {
    checkNoEntry();
    // PlayerController->StartNewConversation(true);
  }
}

void URootWidget::ShowLoadingScreen()
{
  LoadingScreen2->Show();
}

void URootWidget::HideLoadingScreen()
{
  LoadingScreen2->Hide();
}

void URootWidget::OnLanguageSwitch(EAvatarLanguage Language)
{
  ShowLoadingScreen();
  HideUserTextBox();
  HideSuggestionsText();
  HideAvatarMessage();
  StatusMessge->SetVisibility(ESlateVisibility::Collapsed);
  UserMessage->SetVisibility(ESlateVisibility::Collapsed);
  OnLanguageSwitchEvent(Language);
}

EAvatarLanguage URootWidget::GetAvatarLanguage()
{
  if (!GetController())
  {
    return EAvatarLanguage::PL;
  }
  return PlayerController->Language;
}

FText URootWidget::GetTranslation(FName Key)
{
  if (TranslationsTable == nullptr)
  {
    ULog::Error("Translation table not set in root widget.");
    return FText();
  }

  if (TranslationsTable->GetRowMap().Num() <= 0)
  {
    ULog::Error("Translation table is empty.");
    return FText();
  }

  static const FString ContextString(TEXT("ContextString"));
  FTranslationsTableRow* Row = TranslationsTable->FindRow<FTranslationsTableRow>(Key, ContextString);

  if (Row != nullptr)
  {
    switch (GetAvatarLanguage())
    {
    case EAvatarLanguage::PL:
      return Row->PL;
    case EAvatarLanguage::EN:
      return Row->EN;
    }
  }

  ULog::Error("Could not find row in translation table: " + Key.ToString());
  return FText::FromString("No Translation Found");
}

void URootWidget::ShowRecordingInProgressMessage()
{
  if (StatusMessge == nullptr)
  {
    ULog::Error("StatusMessage == nullptr.");
    return;
  }

  StatusMessge->SetText(GetTranslation("StateListeningMessage"));
  StatusMessge->SetVisibility(ESlateVisibility::Visible);
}

void URootWidget::ShowPressToTalkMessage()
{
  if (StatusMessge == nullptr)
  {
    ULog::Error("StatusMessage == nullptr.");
    return;
  }

  StatusMessge->SetText(GetTranslation("StateIdleMessage"));
  StatusMessge->SetVisibility(ESlateVisibility::Visible);
}

void URootWidget::SetStateMessage(EAvatarState State)
{
  if (StatusMessge == nullptr)
  {
    ULog::Error("StatusMessage == nullptr.");
    return;
  }

  // clang-format off
  switch (State)
  {
    case EAvatarState::Idle:      StatusMessge->SetText(GetTranslation("StateIdleMessage")); break;
    case EAvatarState::Listening: StatusMessge->SetText(GetTranslation("StateListeningMessage")); break;
    case EAvatarState::Thinking:  StatusMessge->SetText(GetTranslation("StateThinkingMessage")); break;
    case EAvatarState::Talking:   StatusMessge->SetText(GetTranslation("StateTalkingMessage")); break;
    default: break;
  }
  // clang-format on

  StatusMessge->SetVisibility(ESlateVisibility::Visible);
}

void URootWidget::HideStatusMessage()
{
  if (StatusMessge == nullptr)
  {
    ULog::Error("StatusMessage == nullptr.");
    return;
  }
  StatusMessge->SetVisibility(ESlateVisibility::Collapsed);
}

void URootWidget::HideUserMessage(const float Delay)
{
  if (Delay != 0.0f)
  {
    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] { this->HideUserMessage(0.0); }), 1.0f, false);
    return;
  }

  if (StatusMessge == nullptr)
  {
    ULog::Error("StatusMessage == nullptr.");
    return;
  }
  UserMessage->SetVisibility(ESlateVisibility::Collapsed);
}

void URootWidget::ShowAvatarMessage(FString Message)
{
  if (AvatarMessage == nullptr)
  {
    ULog::Error("AvatarMessage == nullptr.");
    return;
  }
  AvatarMessage->SetText(FText::FromString(Message));
  AvatarMessage->SetVisibility(ESlateVisibility::Visible);
}

void URootWidget::HideAvatarMessage(const float Delay)
{
  if (Delay != 0.0)
  {
    FTimerHandle Handle;
    GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&] { this->HideAvatarMessage(0.0); }), 1.0f, false);
    return;
  }

  if (AvatarMessage == nullptr)
  {
    ULog::Error("AvatarMessage == nullptr.");
    return;
  }
  AvatarMessage->SetVisibility(ESlateVisibility::Collapsed);
}

void URootWidget::SelectThumbnail(AActor* SelectedAvatar)
{
  IAiIdentityInterface* SelectedWithIdentity = Cast<IAiIdentityInterface>(SelectedAvatar);
  if (SelectedWithIdentity == nullptr)
  {
    FString Message = FString::Printf(TEXT("SelectedAvatar is not IAiIdentityInterface: %s"), *SelectedAvatar->GetName());
    UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
    // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Message);
    return;
  }

  for (UCharacterThumbnailWidget* Thumb : AvatarsThumbnails)
  {
    if (!Thumb->Avatar)
    {
      FString Message = FString::Printf(TEXT("Thumb->Avatar is nullptr: %s"), *Thumb->GetName());
      UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
      // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Message);
      continue;
    }

    IAiIdentityInterface* ThumbWithIdentity = Cast<IAiIdentityInterface>(Thumb->Avatar);
    if (ThumbWithIdentity == nullptr)
    {
      FString Message = FString::Printf(TEXT("Thumb->Avatar is not IAiIdentityInterface: %s"), *Thumb->Avatar->GetName());
      UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
      // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Message);
      continue;
    }
    Thumb->UpdateSelectionNative(ThumbWithIdentity->GetIdentity().Name == SelectedWithIdentity->GetIdentity().Name);
  }
}

void URootWidget::OnCharacterThumbnailClick(AActor* SelectedAvatar)
{
  SelectThumbnail(SelectedAvatar);

  if (OnCharacterThumbnailClickEvent.IsBound())
  {
    OnCharacterThumbnailClickEvent.Broadcast(SelectedAvatar);
  }
}

void URootWidget::CreateAvatarsThumbnails(TArray<AAvatarPawn*> SpawnedAvatars)
{
  if (!GetController()) return;
  if (ULog::ErrorIf(ThumbsClass == nullptr, TEXT("No widget class selected for avatar Thumbnail."))) return;

  CharactersThumbnails->ClearChildren();
  for (int32 i = 0; i < SpawnedAvatars.Num(); i++)
  {
    AAvatarPawn* Avatar = SpawnedAvatars[i];
    // UCharacterThumbnailWidget* ThumbWidget = AvatarsThumbnails[i];
    UCharacterThumbnailWidget* ThumbWidget = CreateWidget<UCharacterThumbnailWidget>(PlayerController, ThumbsClass);
    if (ULog::ErrorIf(ThumbWidget == nullptr, TEXT("Failed to create widget for avatar."))) continue;

    if (Avatar != nullptr && ThumbWidget != nullptr)
    {
      ThumbWidget->Avatar = Avatar;
      ThumbWidget->UpdateImage(Avatar->AvatarData.ThumbnailImage);
      ThumbWidget->OnClickEvent.AddUObject(this, &URootWidget::OnCharacterThumbnailClick);
      CharactersThumbnails->AddChild(ThumbWidget);
    }
  }
}

FReply URootWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
  if (bLogKeyDown) ULog::InfoIf(GetController(), "NativeOnPreviewKeyDown: " + InKeyEvent.GetKey().GetFName().ToString());

  if (InKeyEvent.GetKey().GetFName() == FName(TEXT("Enter")))
  {
    OnSendTextButtonClick();
  }

  // Sending unhandled allows to keep using "Enter" to open the text box.
  return FReply::Unhandled();
}

void URootWidget::OnViewportResized(FViewport* Viewport, uint32 Index)
{
  if (!Viewport)
  {
    return;
  }

  FVector2D CurrentViewportSize = FVector2D(Viewport->GetSizeXY());

  if (!LastViewportSize.Equals(CurrentViewportSize))
  {
    LastViewportSize = CurrentViewportSize;
    FString Message = FString::Printf(TEXT("Viewport size changed to: %s"), *CurrentViewportSize.ToString());
    // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, Message);
    UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
  }
}
