// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/OptionsMenu.h"
#include "Components/Spacer.h"

#include "TextButtonWidget.h"
#include "WidgetBase.h"

void UOptionsMenu::NativeConstruct()
{
  Super::NativeConstruct();
  BuildPanels();
}

void UOptionsMenu::SynchronizeProperties()
{
  Super::SynchronizeProperties();
  BuildPanels();
}

void UOptionsMenu::BuildPanels()
{
  OptionsContainer->ClearChildren();
  MenuContainer->ClearChildren();
  UWidgetBase* FirstPanel = nullptr;
  for (int32 Index = 0; Index < OptionsPanels.Num(); ++Index)
  {
    FPanelSettings& OptionsPanel = OptionsPanels[Index];
    if (!OptionsPanel.PanelWidgetClass)
    {
      UE_LOG(LogTemp, Error, TEXT("UOptionsMenu::BuildPanels: PanelWidgetClass is not set"));
      return;
    }
    UWidgetBase* Panel = CreateWidget<UWidgetBase>(this, OptionsPanel.PanelWidgetClass);
    if (Index == 0) FirstPanel = Panel;
    if (Panel == nullptr)
    {
      UE_LOG(
          LogTemp, Error, TEXT("UOptionsMenu::BuildPanels: Failed to create widget of class %s"), *OptionsPanel.PanelWidgetClass->GetName()
      );
      return;
    }

    Panel->SetInteractive(true);
    Panel->SetToggle(false);
    OptionsContainer->AddChild(Panel);

    if (!OptionsPanel.ButtonWidgetClass)
    {
      UE_LOG(LogTemp, Error, TEXT("UOptionsMenu::BuildPanels: ButtonWidgetClass is not set"));
      return;
    }
    UButtonBaseWidget* Button = CreateWidget<UButtonBaseWidget>(this, OptionsPanel.ButtonWidgetClass);
    if (Button == nullptr)
    {
      UE_LOG(
          LogTemp, Error, TEXT("UOptionsMenu::BuildPanels: Failed to create widget of class %s"), *OptionsPanel.ButtonWidgetClass->GetName()
      );
      return;
    }

    if (Index != 0)
    {
      USpacer* Spacer = NewObject<USpacer>(this);
      Spacer->SetSize(FVector2D(MenuItemsSpacing, MenuItemsSpacing));
      MenuContainer->AddChild(Spacer);
    }

    Button->SetInteractive(true);
    if (UTextButtonWidget* TextButton = Cast<UTextButtonWidget>(Button))
    {
      TextButton->SetText(OptionsPanel.Label);
    }
    Button->SetControlledWidget(Panel);
    Button->Button->OnClicked.AddDynamic(this, &UOptionsMenu::HideAllPanels);
    Button->Button->OnClicked.AddDynamic(Panel, &UWidgetBase::Toggle);
    Button->SetToggle(true);
    MenuContainer->AddChild(Button);

    // log MenuContainer children count
  }
  if (FirstPanel)
  {
    FirstPanel->SetToggle(true);
  }
}

void UOptionsMenu::HideAllPanels()
{
  if (OptionsContainer != nullptr)
  {
    for (int32 Index = 0; Index < OptionsContainer->GetChildrenCount(); ++Index)
    {
      UWidget* Widget = OptionsContainer->GetChildAt(Index);
      if (UWidgetBase* WidgetBase = Cast<UWidgetBase>(Widget))
      {
        WidgetBase->SetToggle(false);
      }
    }
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

  GetWorld()->GetTimerManager().SetTimer(MenuTimeoutHandle, FTimerDelegate::CreateLambda([this] { this->Hide(); }), MenuTimeout, false);
  GetWorld()->GetTimerManager().SetTimer(TimeoutTickHandle, this, &UOptionsMenu::OnTimeoutTick, 1.0f, true);
}
