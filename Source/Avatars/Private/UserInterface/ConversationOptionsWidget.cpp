// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/ConversationOptionsWidget.h"

#include "Misc/DefaultValueHelper.h"

#include "AvatarsGame.h"
#include "AvatarsPlayerController.h"
#include "Log.h"
#include "Persistance/PersistanceController.h"

void UConversationOptionsWidget::NativeConstruct()
{
  if (AvatarChangeDelayInput != nullptr)
  {
    AvatarChangeDelayInput->OnTextChanged.AddDynamic(this, &UConversationOptionsWidget::OnAvatarsChangeDelayChange);
  }

  if (IdleGreetingTimeoutInput != nullptr)
  {
    IdleGreetingTimeoutInput->OnTextChanged.AddDynamic(this, &UConversationOptionsWidget::OnIdleGreetingTimeoutChange);
  }

  if (GetController())
  {
    AvatarChangeDelayInput->SetText(PlayerController->ChangeAvatarTimeout.GetDelayAsText());
    IdleGreetingTimeoutInput->SetText(PlayerController->IdleGreetingTimeout.GetDelayAsText());
  }
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
            PlayerController->StartChangeAvatarTimeout();
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
            PlayerController->StartGreetingTimeout();
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