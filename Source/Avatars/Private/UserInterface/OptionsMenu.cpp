// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/OptionsMenu.h"

void UOptionsMenu::NativeConstruct()
{
  if (OpenConversationMenuButton != nullptr)
  {
    OpenConversationMenuButton->OnClicked.AddDynamic(this, &UOptionsMenu::OnOpenConversationMenuButtonClicked);
  }
  if (ConversationOptions)
  {
    HideAllExceptOf(ConversationOptions);
  }
  if (OpenCameraOptionsButton != nullptr)
  {
    OpenCameraOptionsButton->OnClicked.AddDynamic(this, &UOptionsMenu::OnOpenCameraOptionsButtonClicked);
  }
}

void UOptionsMenu::HideAllExceptOf(UWidget* WidgetToKeepVisible)
{
  if (OptionsContainer != nullptr)
  {
    for (int32 Index = 0; Index < OptionsContainer->GetChildrenCount(); ++Index)
    {
      UWidget* Widget = OptionsContainer->GetChildAt(Index);
      if (Widget != nullptr)
      {
        Widget->SetVisibility(ESlateVisibility::Collapsed);
      }
    }
    WidgetToKeepVisible->SetVisibility(ESlateVisibility::Visible);
  }
}

void UOptionsMenu::SetUseTimeout(bool bUse)
{
  bUseTimeout = bUse;
  if (!bUseTimeout) ClearTimeout();
  TimoutDisplay->SetVisibility(bUseTimeout ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UOptionsMenu::OnOpenConversationMenuButtonClicked()
{
  if (ConversationOptions != nullptr)
  {
    HideAllExceptOf(ConversationOptions);
  }
}

void UOptionsMenu::OnOpenCameraOptionsButtonClicked()
{
  if (CameraOptions != nullptr)
  {
    HideAllExceptOf(CameraOptions);
  }
}

void UOptionsMenu::ClearTimeout()
{
  if (MenuTimeoutHandle.IsValid())
  {
    GetWorld()->GetTimerManager().ClearTimer(MenuTimeoutHandle);
  }
  if (TimeoutTickHandle.IsValid())
  {
    GetWorld()->GetTimerManager().ClearTimer(TimeoutTickHandle);
  }
}

void UOptionsMenu::HideNative()
{
  ClearTimeout();
}

void UOptionsMenu::ShowNative()
{
  StartMenuTimeout();
}

void UOptionsMenu::UpdateTimeoutDisplay()
{
  if (TimoutDisplay != nullptr) TimoutDisplay->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), TimoutCounter)));
}

void UOptionsMenu::OnTimeoutTick()
{
  TimoutCounter -= 1.0f;
  if (TimoutCounter <= 0.0f)
  {
    if (TimeoutTickHandle.IsValid()) GetWorld()->GetTimerManager().ClearTimer(TimeoutTickHandle);
    return;
  }
  UpdateTimeoutDisplay();
}

void UOptionsMenu::StartMenuTimeout(const float Timeout)
{
  if (!bUseTimeout) return;

  ClearTimeout();
  MenuTimeout = Timeout;
  TimoutCounter = MenuTimeout;
  UpdateTimeoutDisplay();

  GetWorld()->GetTimerManager().SetTimer(MenuTimeoutHandle, this, &UOptionsMenu::Hide, MenuTimeout, false);
  GetWorld()->GetTimerManager().SetTimer(TimeoutTickHandle, this, &UOptionsMenu::OnTimeoutTick, 1.0f, true);
}
