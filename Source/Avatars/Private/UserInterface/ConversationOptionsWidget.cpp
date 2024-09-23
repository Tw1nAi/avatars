// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/ConversationOptionsWidget.h"

// Engine
#include "Misc/DefaultValueHelper.h"

// Custom
#include "AvatarsGame.h"
#include "AvatarsPlayerController.h"
#include "Log.h"
#include "Persistance/PersistanceController.h"

// Plugins
#include "OpenAIUtils.h"

DEFINE_LOG_CATEGORY(LogConversationOptionsWidget);

void UConversationOptionsWidget::NativeConstruct()
{
  UPersistanceController* Persistance = UAvatarsGame::GetPersistance(GetWorld());
  if (ULog::ErrorIf(Persistance == nullptr, "UConversationOptionsWidget::NativeConstruct: Persistance is not valid")) return;

  if (!GetController()) return;

  AvatarChangeDelayInput->SetText(PlayerController->ChangeAvatarTimeout.GetDelayAsText());
  IdleGreetingTimeoutInput->SetText(PlayerController->IdleGreetingTimeout.GetDelayAsText());
  TranscriptionApiComboBox->ClearOptions();
  TranscriptionApiComboBox->AddOption(FConversationSettings::RestApiName);
  TranscriptionApiComboBox->AddOption(FConversationSettings::OpenAiApiName);

  FString Api = PlayerController->bUseLocalTranscription ? FConversationSettings::RestApiName : FConversationSettings::OpenAiApiName;
  TranscriptionApiComboBox->SetSelectedOption(Api);

  ApiKey->SetText(FText::FromString(Persistance->ConversationSettings.GetOpenApiKey()));

  AvatarChangeDelayInput->OnTextChanged.AddDynamic(this, &UConversationOptionsWidget::OnAvatarsChangeDelayChange);
  IdleGreetingTimeoutInput->OnTextChanged.AddDynamic(this, &UConversationOptionsWidget::OnIdleGreetingTimeoutChange);
  TranscriptionApiComboBox->OnSelectionChanged.AddDynamic(this, &UConversationOptionsWidget::OnTranscriptionApiChange);
  ApiKey->OnTextChanged.AddDynamic(this, &UConversationOptionsWidget::OnApiKeyChange);
}

bool UConversationOptionsWidget::GetController()
{
  PlayerController = AAvatarsPlayerController::Get(GetWorld());
  ULog::ErrorIf(PlayerController == nullptr, "PlayerController is not valid.");
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

  if (!GetController()) return;

  PlayerController->SetUseLocalTranscription(Api == FConversationSettings::RestApiName);
  Persistance->ConversationSettings.SetTranscriptionApi(Api);
  Persistance->SaveAll();
  LastSavedApi = Api;
  UE_LOG(LogConversationOptionsWidget, Display, TEXT("Transcription API changed to %s"), *Api);

  if (Api == FConversationSettings::OpenAiApiName)
  {
    OpenAiOptions->SetVisibility(ESlateVisibility::Visible);
    ApiKey->SetText(FText::FromString(Persistance->ConversationSettings.GetOpenApiKey()));
  }
  else
  {
    OpenAiOptions->SetVisibility(ESlateVisibility::Collapsed);
  }
}

void UConversationOptionsWidget::OnTranscriptionApiChange(FString ApiString, ESelectInfo::Type SelectionType)
{
  SetTranscriptionApi(ApiString);
}

void UConversationOptionsWidget::OnApiKeyChange(const FText& InText)
{
  // TODO: validate api key
  // GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::White, TEXT("OnApiKeyChange: ") + InText.ToString());

  UWorld* World = GetWorld();
  if (ULog::ErrorIf(World == nullptr, "World is not valid.")) return;

  UPersistanceController* Persistance = UAvatarsGame::GetPersistance(World);
  if (ULog::ErrorIf(Persistance == nullptr, "Persistance is not valid")) return;

  ChangeApiKeyDebounce(
      [=, WeakThis = TWeakObjectPtr<UConversationOptionsWidget>(this)]() {
        if (ULog::ErrorIf(WeakThis == nullptr, "WeakThis is not valid")) return;

        FString ApiKey = InText.ToString().TrimStartAndEnd();
        UOpenAIUtils::setOpenAIApiKey(ApiKey);
        Persistance->ConversationSettings.SetOpenApiKey(ApiKey);
        Persistance->SaveAll();
        // log new api key but only first four and four last characters, the rest are *
        FString LogApiKey = ApiKey;
        if (LogApiKey.Len() > 8)
        {
          LogApiKey = LogApiKey.Left(4) + FString::ChrN(LogApiKey.Len() - 8, '*') + LogApiKey.Right(4);
        }
        UE_LOG(LogConversationOptionsWidget, Display, TEXT("OpenAI API key changed to %s"), *LogApiKey);
      },
      World
  );
}