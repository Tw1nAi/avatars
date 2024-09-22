// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/ConversationOptionsWidget.h"

#include "Misc/DefaultValueHelper.h"

#include "AvatarsGame.h"
#include "AvatarsPlayerController.h"
#include "Log.h"
#include "Persistance/PersistanceController.h"

DEFINE_LOG_CATEGORY(LogConversationOptionsWidget);

void UConversationOptionsWidget::NativeConstruct()
{
  if (ULog::ErrorIf(IdleGreetingTimeoutInput == nullptr, "IdleGreetingTimeoutInput is not valid")) return;
  if (ULog::ErrorIf(AvatarChangeDelayInput == nullptr, "AvatarChangeDelayInput is not valid")) return;
  if (ULog::ErrorIf(TranscriptionApiComboBox == nullptr, "TranscriptionApiComboBox is not valid")) return;

  AvatarChangeDelayInput->OnTextChanged.AddDynamic(this, &UConversationOptionsWidget::OnAvatarsChangeDelayChange);

  IdleGreetingTimeoutInput->OnTextChanged.AddDynamic(this, &UConversationOptionsWidget::OnIdleGreetingTimeoutChange);

  if (ULog::ErrorIf(!GetController(), "Could not get player controller")) return;

  AvatarChangeDelayInput->SetText(PlayerController->ChangeAvatarTimeout.GetDelayAsText());
  IdleGreetingTimeoutInput->SetText(PlayerController->IdleGreetingTimeout.GetDelayAsText());
  TranscriptionApiComboBox->ClearOptions();
  TranscriptionApiComboBox->AddOption(FConversationSettings::RestApiName);
  TranscriptionApiComboBox->AddOption(FConversationSettings::OpenAiApiName);

  if (UPersistanceController* Persistance = UAvatarsGame::GetPersistance(GetWorld()))
  {
    FString Api = Persistance->ConversationSettings.TranscriptionApi;
    if (Api.IsEmpty())
    {
      Api = FString(FConversationSettings::RestApiName);
    }
    TranscriptionApiComboBox->SetSelectedOption(Api);
    SetTranscriptionApi(Api);
  }

  TranscriptionApiComboBox->OnSelectionChanged.AddDynamic(this, &UConversationOptionsWidget::OnTranscriptionApiChange);
}

bool UConversationOptionsWidget::GetController()
{
  PlayerController = AAvatarsPlayerController::Get(GetWorld());
  return PlayerController != nullptr;
}

void UConversationOptionsWidget::OnAvatarsChangeDelayChange(const FText& InText)
{
  FString OriginalString = InText.ToString().TrimStartAndEnd();

  int32 FirstDotIndex = OriginalString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromStart);
  if (FirstDotIndex != INDEX_NONE)
  {
    int32 SecondDotIndex = OriginalString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromStart, FirstDotIndex + 1);
    if (SecondDotIndex != FirstDotIndex && SecondDotIndex != INDEX_NONE)
    {
      OriginalString = OriginalString.Left(SecondDotIndex);
    }
  }

  FString FilteredString;

  // Iterate through each character in the original string
  for (TCHAR Char : OriginalString)
  {
    // Check if the character is a digit
    if (FChar::IsDigit(Char) || Char == *".")
    {
      FilteredString.AppendChar(Char);
    }
  }

  if (!FilteredString.IsEmpty() && FilteredString[0] == *".")
  {
    FilteredString = TEXT("0") + FilteredString;
  }

  AvatarChangeDelayInput->SetText(FText::FromString(FilteredString.Left(4)));

  float Delay = 0.f;
  // check if text input has any digits and can be vonverted to flaot
  if (!FDefaultValueHelper::ParseFloat(FilteredString, Delay))
  {
    ULog::Warn("Could not parse text input into float");
    // return;
  }

  if (Delay < 0.0f)
  {
    Delay = 0.0f;
  }

  UWorld* World = GetWorld();
  if (ULog::ErrorIf(World == nullptr, "World is not valid.")) return;

  ChangeAvatarDebounce(
      [=, WeakThis = TWeakObjectPtr<UConversationOptionsWidget>(this)]() {
        if (ULog::ErrorIf(WeakThis == nullptr, "WeakThis is not valid")) return;

        if (WeakThis->GetController())
        {
          FTimeoutManager& Manager = WeakThis->PlayerController->ChangeAvatarTimeout;
          Manager.SetTimeoutDelay(Delay);

          if (UPersistanceController* Persistance = UAvatarsGame::GetPersistance(WeakThis->GetWorld()))
          {
            // TODO: figure out better way of doint this, maybe consider GameplayTags
            Persistance->ConversationSettings.AvatarChangeTimeout = Delay;
            Persistance->SaveAll();
          }

          if (Delay > 0.0f)
          {
            WeakThis->PlayerController->StartChangeAvatarTimeout();
          }
          else
          {
            Manager.ClearIfValid(WeakThis->GetWorld());
          }
        }
      },
      World
  );
}

void UConversationOptionsWidget::OnIdleGreetingTimeoutChange(const FText& InText)
{
  FString OriginalString = InText.ToString().TrimStartAndEnd();

  int32 FirstDotIndex = OriginalString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromStart);
  if (FirstDotIndex != INDEX_NONE)
  {
    int32 SecondDotIndex = OriginalString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromStart, FirstDotIndex + 1);
    if (SecondDotIndex != FirstDotIndex && SecondDotIndex != INDEX_NONE)
    {
      OriginalString = OriginalString.Left(SecondDotIndex);
    }
  }

  FString FilteredString;

  // Iterate through each character in the original string
  for (TCHAR Char : OriginalString)
  {
    // Check if the character is a digit
    if (FChar::IsDigit(Char) || Char == *".")
    {
      FilteredString.AppendChar(Char);
    }
  }

  if (!FilteredString.IsEmpty() && FilteredString[0] == *".")
  {
    FilteredString = TEXT("0") + FilteredString;
  }

  IdleGreetingTimeoutInput->SetText(FText::FromString(FilteredString.Left(4)));

  float Delay = 0.f;
  // check if text input has any digits and can be vonverted to flaot
  if (!FDefaultValueHelper::ParseFloat(FilteredString, Delay))
  {
    ULog::Warn("Could not parse text input into float");
    // return;
  }

  if (Delay < 0.0f)
  {
    Delay = 0.0f;
  }

  UWorld* World = GetWorld();
  if (ULog::ErrorIf(World == nullptr, "World is not valid.")) return;

  IdleGreetingTimeout(
      [=, WeakThis = TWeakObjectPtr<UConversationOptionsWidget>(this)]() {
        if (ULog::ErrorIf(WeakThis == nullptr, "WeakThis is not valid")) return;

        if (WeakThis->GetController())
        {
          FTimeoutManager& Manager = WeakThis->PlayerController->IdleGreetingTimeout;
          Manager.SetTimeoutDelay(Delay);

          if (UPersistanceController* Persistance = UAvatarsGame::GetPersistance(WeakThis->GetWorld()))
          {
            // TODO: figure out better way of doint this, maybe consider GameplayTags
            Persistance->ConversationSettings.IdleGreetingTimeout = Delay;
            Persistance->SaveAll();
          }

          if (Delay > 0.0f)
          {
            WeakThis->PlayerController->StartGreetingTimeout();
          }
          else
          {
            Manager.ClearIfValid(WeakThis->GetWorld());
          }
        }
      },
      World
  );
}

void UConversationOptionsWidget::SetTranscriptionApi(const FString& ApiString)
{
  if (ApiString.IsEmpty())
  {
    ULog::Warn("Transcription Api settings combo box submited empty string.");
    return;
  }

  if (ApiString == LastSavedApi)
  {
    return;
  }

  UWorld* World = GetWorld();
  if (ULog::ErrorIf(World == nullptr, "World is not valid.")) return;

  UPersistanceController* Persistance = UAvatarsGame::GetPersistance(World);
  if (ULog::ErrorIf(Persistance == nullptr, "Persistance is not valid")) return;

  FString Api = ApiString;
  if (Api.IsEmpty())
  {
    Api = FConversationSettings::RestApiName;
  }

  if (!GetController())
  {
    ULog::Error("Could not get player controller");
    return;
  }

  Persistance->ConversationSettings.SetTranscriptionApi(Api);
  PlayerController->SetUseLocalTranscription(Api == FConversationSettings::RestApiName);
  Persistance->SaveAll();
  LastSavedApi = Api;
  UE_LOG(LogConversationOptionsWidget, Display, TEXT("Transcription API changed to %s"), *Api);
  return;
}

void UConversationOptionsWidget::OnTranscriptionApiChange(FString ApiString, ESelectInfo::Type SelectionType)
{
  SetTranscriptionApi(ApiString);
}