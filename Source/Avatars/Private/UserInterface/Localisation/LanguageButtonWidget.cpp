// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/Localisation/LanguageButtonWidget.h"

#include "Get.h"

void ULanguageButtonWidget::NativeConstruct()
{
  Super::NativeConstruct();

  SetIsFocusable(false);

  if (Button != nullptr)
  {
    Button->OnPressed.Clear();
    Button->OnPressed.AddDynamic(this, &ULanguageButtonWidget::OnButtonPressed);
  }
}

void ULanguageButtonWidget::SynchronizeProperties()
{
  Super::SynchronizeProperties();

  if (LanguageText != nullptr)
  {
    LanguageText->SetText(FText::FromString(FGet::LanguageEnumAsIsoString(Language)));
  }
}

void ULanguageButtonWidget::SetLanguage(EAvatarLanguage InLanguage)
{
  Language = InLanguage;
}

void ULanguageButtonWidget::OnButtonPressed()
{
  if (OnLanguageSwitchEvent.IsBound()) OnLanguageSwitchEvent.Broadcast(Language);
}

void ULanguageButtonWidget::SetSelected(const bool bInSelected)
{
  OnSelectedChange(bInSelected);
}
