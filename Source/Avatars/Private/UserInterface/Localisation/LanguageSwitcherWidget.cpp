// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/Localisation/LanguageSwitcherWidget.h"

#include "AvatarsPlayerController.h"
#include "UserInterface/Localisation/LanguageButtonWidget.h"

void ULanguageSwitcherWidget::NativeConstruct()
{
  Super::NativeConstruct();

  SetIsFocusable(false);
}

void ULanguageSwitcherWidget::SynchronizeProperties()
{
  Super::SynchronizeProperties();
  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("SynchronizeProperties"));
  if (LanguageButtonClass == nullptr) return;

  ButtonsBox->ClearChildren();

  for (EAvatarLanguage Language : Languages)
  {
    if (HasLanguageButton(Language)) continue;

    ULanguageButtonWidget* Button = NewObject<ULanguageButtonWidget>(this, LanguageButtonClass);
    Button->SetLanguage(Language);
    Button->OnLanguageSwitchEvent.Clear();
    Button->OnLanguageSwitchEvent.AddDynamic(this, &ULanguageSwitcherWidget::OnLanguageButtonPressed);
    LanguageButtons.Add(Button);
    ButtonsBox->AddChild(Button);

    AAvatarsPlayerController* PlayerController = Cast<AAvatarsPlayerController>(GetOwningPlayer());
    if (PlayerController != nullptr) Button->SetSelected(Button->IsLanguage(PlayerController->Language));
  }
}

void ULanguageSwitcherWidget::OnLanguageButtonPressed(EAvatarLanguage Language)
{
  if (OnLanguageSwitchEvent.IsBound()) OnLanguageSwitchEvent.Broadcast(Language);

  for (ULanguageButtonWidget* Button : LanguageButtons)
  {
    Button->SetSelected(Button->IsLanguage(Language));
  }

  AAvatarsPlayerController* PlayerController = Cast<AAvatarsPlayerController>(GetOwningPlayer());
  if (PlayerController != nullptr) PlayerController->SetLanguage(Language);
}

bool ULanguageSwitcherWidget::HasLanguageButton(EAvatarLanguage Language) const
{
  for (ULanguageButtonWidget* Button : LanguageButtons)
  {
    if (Button->IsLanguage(Language)) return true;
  }

  return false;
}