// Copyright Juice sp. z o. o., All Rights Reserved.

#include "WidgetBase.h"

void UWidgetBase::NativeConstruct()
{
  Super::NativeConstruct();
  SetInteractive(true);

  bIsToggled = !IsCollapsed() && !IsHidden();
}

void UWidgetBase::ShowNative()
{
  // implement in child classes
}

void UWidgetBase::HideNative()
{
  // implement in child classes
}

void UWidgetBase::Show()
{
  bIsToggled = true; // side effect!!
  if (bIsInteractive)
  {
    SetVisibility(ESlateVisibility::Visible);
  }
  else
  {
    ShowNotInteractive();
  }
  ShowNative();
}

void UWidgetBase::Hide(const bool bShouldCollapse /*= true*/)
{
  bIsToggled = false; // side effect!!
  SetVisibility(bShouldCollapse ? ESlateVisibility::Collapsed : ESlateVisibility::Hidden);
  HideNative();
}

void UWidgetBase::ShowNotInteractive(const bool bAllowChildrenInteractive /*= true*/)
{
  bIsToggled = true;      // side effect!!
  bIsInteractive = false; // side effect!!
  SetVisibility(bAllowChildrenInteractive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::SelfHitTestInvisible);
  ShowNative();
}

void UWidgetBase::SetInteractive(const bool bInteractive, const bool bUpdateVisibility /*= true*/)
{
  bIsInteractive = bInteractive;
  if (!bUpdateVisibility) return;

  if (IsVisible())
  {
    Show();
    return;
  }

  Hide();
}

bool UWidgetBase::IsCollapsed() const
{
  return GetVisibility() == ESlateVisibility::Collapsed;
}

bool UWidgetBase::IsHidden() const
{
  return GetVisibility() == ESlateVisibility::Hidden;
}

bool UWidgetBase::IsInteractive() const
{
  return bIsInteractive;
}

void UWidgetBase::SetToggle(const bool bToggle)
{
  bIsToggled = bToggle;
  if (bIsToggled)
  {
    Show();
  }
  else
  {
    Hide();
  }
}

void UWidgetBase::Toggle()
{
  SetToggle(!bIsToggled);
}