// Copyright Juice sp. z o. o., All Rights Reserved.

#include "ButtonBaseWidget.h"

void UButtonBaseWidget::NativeConstruct()
{
  Super::NativeConstruct();

  if (Button != nullptr)
  {
    Button->OnPressed.Clear();
    Button->OnPressed.AddDynamic(this, &UButtonBaseWidget::OnPressedDefault);
    Button->OnReleased.Clear();
    Button->OnReleased.AddDynamic(this, &UButtonBaseWidget::OnReleasedDefault);
  }
}

void UButtonBaseWidget::SynchronizeProperties()
{
  Super::SynchronizeProperties();

  if (Button != nullptr)
  {
    Button->SetStyle(ButtonStyle);
  }
}

void UButtonBaseWidget::OnPressedDefault()
{
  if (!bUseOnHoldEvent || !OnHoldEvent.IsBound()) return;

  // Create the ticker object
  if (!HoldTicker)
  {
    HoldTicker = MakeUnique<FButtonHoldTicker>(
        [WeakButton = TWeakObjectPtr<UButtonBaseWidget>(this)]() {
          if (!WeakButton.IsValid())
          {
            UE_LOG(LogTemp, Error, TEXT("Button is not valid"));
            return;
          }

          WeakButton->OnHoldEvent.Broadcast();
        },
        OnHoldInterval
    );
  }
}

void UButtonBaseWidget::OnReleasedDefault()
{
  // Destroy the ticker object
  HoldTicker.Reset();
}

void UButtonBaseWidget::SetControlledWidget(UWidgetBase* Widget)
{
  ControlledWidget = Widget;
}