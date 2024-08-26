// Copyright Juice sp. z o. o., All Rights Reserved.

#include "WidgetBase.h"

void UWidgetBase::SetInteractive(const bool bInteractive, const bool bUpdateVisibility /*= true*/)
{
  bIsInteractive = bInteractive;
  if (!bUpdateVisibility) return;

  ESlateVisibility Current = GetVisibility();
  if (Current == ESlateVisibility::Visible || Current == ESlateVisibility::HitTestInvisible)
  {
    SetVisibility(bIsInteractive ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible);
  }
}

void UWidgetBase::Show()
{
  SetVisibility(bIsInteractive ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible);
}

void UWidgetBase::ShowNotInteractive(const bool bAllowChildreInteractive /*= true*/)
{
  bIsInteractive = false; // side effect!!
  SetVisibility(bAllowChildreInteractive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::SelfHitTestInvisible);
}

bool UWidgetBase::IsCollapsed() const
{
  return GetVisibility() == ESlateVisibility::Collapsed;
}

void UWidgetBase::Hide(const bool bShouldCollapse /*= true*/)
{
  SetVisibility(bShouldCollapse ? ESlateVisibility::Collapsed : ESlateVisibility::Hidden);
}

bool UWidgetBase::IsHidden() const
{
  return GetVisibility() == ESlateVisibility::Hidden;
}

bool UWidgetBase::IsInteractive() const
{
  return bIsInteractive;
}