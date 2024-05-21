// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/RootWidget.h"

#include "AvatarsPlayerController.h"
#include "Components/GridSlot.h"
#include "Get.h"
#include "Log.h"
#include "Translations.h"
#include "UserInterface/LoadingScreenWidget.h"

void URootWidget::NativeConstruct()
{
  Super::NativeConstruct();

  if (StartConversationButton != nullptr)
  {
    StartConversationButton->OnClicked.AddDynamic(this, &URootWidget::StartNewConversation);
  };
  if (BackToCharacterSelectionButton != nullptr)
  {
    BackToCharacterSelectionButton->OnClicked.AddDynamic(this, &URootWidget::OnBackToCharacterSelection);
  }
  if (SendTextButton != nullptr)
  {
    SendTextButton->OnClicked.AddDynamic(this, &URootWidget::OnSendTextButtonClick);
  }

  if (LanguageSwitcher != nullptr)
  {
    LanguageSwitcher->OnLanguageSwitchEvent.AddDynamic(this, &URootWidget::OnLanguageSwitch);
  }

  WidgetsHiddenByDefault.Add(StatusMessge);
  WidgetsHiddenByDefault.Add(BackToCharacterSelectionButton);
  WidgetsHiddenByDefault.Add(UserTextInputBox);
  WidgetsHiddenByDefault.Add(OptionsMenu);

  for (UWidget* Widget : WidgetsHiddenByDefault)
  {
    if (Widget != nullptr) Widget->SetVisibility(ESlateVisibility::Collapsed);
  }

  AvatarsThumbnails.Add(JanZumbachThumbnail2);
  AvatarsThumbnails.Add(JakKowalewskiThumbnail2);
  AvatarsThumbnails.Add(WojtekTheBearThumbnail);

  HideSuggestionsText();
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
  if (Suggestions != nullptr)
  {
    Suggestions->SetVisibility(ESlateVisibility::Visible);
  }
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
    SuggestionsText->SetVisibility(ESlateVisibility::Visible);
  }
}

void URootWidget::OnBackToCharacterSelection()
{
  if (GetController())
  {
    PlayerController->StopDialog();
  }

  UserTextInputBox->SetVisibility(ESlateVisibility::Collapsed);
  BackToCharacterSelectionButton->SetVisibility(ESlateVisibility::Collapsed);
  StatusMessge->SetVisibility(ESlateVisibility::Collapsed);
  UserMessage->SetVisibility(ESlateVisibility::Collapsed);

  StartConversationButton->SetVisibility(ESlateVisibility::Visible);
  CharacterSelection->SetVisibility(ESlateVisibility::Visible);
}

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
    if (bDebug) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("OnSendTextButtonClick: found command: ") + Command);

    if (Command.Contains(PlayerController->ServicePin) && Command.Contains("options"))
    {
      if (bDebug)
      {
        Command = Command.Replace(*PlayerController->ServicePin, TEXT("")).TrimStartAndEnd();
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, (TEXT("OnSendTextButtonClick: executing command: ") + Command));
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
  UserTextInputBox->SetVisibility(ESlateVisibility::Visible);
  StatusMessge->SetVisibility(ESlateVisibility::Collapsed);
  UserMessage->SetVisibility(ESlateVisibility::Collapsed);

  UserTextBox->SetKeyboardFocus();
}

void URootWidget::DisplayUserMessage(FString Message)
{
  verify(UserMessage != nullptr);

  if (UserMessage != nullptr)
  {
    Message.TrimStartAndEndInline();
    UserMessage->SetText(FText::FromString(Message.Left(1).ToUpper() + Message.RightChop(1)));
    UserMessage->SetVisibility(ESlateVisibility::Visible);
  }
}

void URootWidget::StartNewConversation()
{
  if (GetController())
  {
    PlayerController->StartNewConversation(true);
  }
  StartConversationButton->SetVisibility(ESlateVisibility::Collapsed);
  // CharacterSelection->SetVisibility(ESlateVisibility::Collapsed);
  // BackToCharacterSelectionButton->SetVisibility(ESlateVisibility::Visible);
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
  StatusMessge->SetVisibility(ESlateVisibility::Collapsed);
  UserMessage->SetVisibility(ESlateVisibility::Collapsed);
  SetLanguage(Language);
}

EAvatarLanguage URootWidget::GetAvatarLanguage()
{
  if (!GetController())
  {
    return EAvatarLanguage::PL;
  }
  return PlayerController->Language;
}

void URootWidget::SetLanguage(EAvatarLanguage InLanguage)
{
  if (!GetController())
  {
    return;
  }
  PlayerController->SetLanguage(InLanguage);
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

  StatusMessge->SetVisibility(ESlateVisibility::Visible);
  StatusMessge->SetText(GetTranslation("RecordingInProgressMessage"));
}

void URootWidget::ShowPressToTalkMessage()
{
  if (StatusMessge == nullptr)
  {
    ULog::Error("StatusMessage == nullptr.");
    return;
  }

  StatusMessge->SetVisibility(ESlateVisibility::Visible);
  StatusMessge->SetText(GetTranslation("PressToTalkMessage"));
}

void URootWidget::HideUserMessage(const float Delay = 0.0)
{
  if (Delay != 0.0)
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

void URootWidget::SelectThumbnailById(const FAvatarId& Id)
{
  for (UCharacterThumbnailWidget* Thumb : AvatarsThumbnails)
  {
    Thumb->bIsSelected = Thumb->AvatarId == Id;
  }
}

void URootWidget::UpdateThumbnailsDisplay()
{
  // for (UCharacterThumbnailWidget* Thumb : AvatarsThumbnails)
  // {
  //   Thumb->SetVisibility(Thumb->bIsSelected ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
  //   // Thumb->UpdateSelection(Thumb->AvatarId == Id || Thumb->Id_v2 == Id_v2);
  // }
}

void URootWidget::OnCharacterThumbnailClick(const FAvatarId& Id)
{
  SelectThumbnailById(Id);
  UpdateThumbnailsDisplay();

  if (OnCharacterThumbnailClickEvent.IsBound())
  {
    OnCharacterThumbnailClickEvent.Broadcast(Id);
    HideSuggestionsText();
  }
}

void URootWidget::CreateAvatarsThumbnails(TArray<FAvatarData> AvatarsData)
{
  if (ULog::ErrorIf(AvatarsThumbnails.Num() == 0, TEXT("No AvatarsThumbnails"))) return;

  for (UCharacterThumbnailWidget* ThumbWidget : AvatarsThumbnails)
  {
    for (FAvatarData& Avatar : AvatarsData)
    {
      if (ThumbWidget->CharacterName == Avatar.Name)
      {
        ThumbWidget->AvatarId = Avatar.Id;
        ThumbWidget->CharacterName = Avatar.Name;
        ThumbWidget->UpdateImage(Avatar.ThumbnailImage);
        ThumbWidget->OnClickEvent.AddUObject(this, &URootWidget::OnCharacterThumbnailClick);
        ThumbWidget->Update();
      }
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
