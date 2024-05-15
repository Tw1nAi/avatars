// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/BaseWidget.h"

void UBaseWidget::ShowNative() {}
void UBaseWidget::Show()
{
  ShowNative();
  SetVisibility(ESlateVisibility::Visible);
}

void UBaseWidget::HideNative() {}
void UBaseWidget::Hide()
{
  HideNative();
  SetVisibility(ESlateVisibility::Collapsed);
}