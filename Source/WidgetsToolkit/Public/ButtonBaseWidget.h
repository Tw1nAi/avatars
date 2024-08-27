// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Components/Button.h"
#include "Components/NamedSlot.h"
#include "CoreMinimal.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Tickable.h"
#include "WidgetBase.h"

#include "ButtonBaseWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonHoldSignature);

class FButtonHoldTicker : public FTickableGameObject
{
public:
  FButtonHoldTicker(TFunction<void()> InCallback, float InHoldInterval)
      : Callback(InCallback), HoldInterval(InHoldInterval), AccumulatedTime(0.0f)
  {
  }

  virtual void Tick(float DeltaTime) override
  {
    AccumulatedTime += DeltaTime;
    while (AccumulatedTime >= HoldInterval)
    {
      AccumulatedTime -= HoldInterval;
      if (Callback)
      {
        Callback();
      }
    }
  }

  virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FButtonHoldTicker, STATGROUP_Tickables); }

  virtual bool IsTickable() const override
  {
    return true; // The object is always tickable when active
  }

  virtual bool IsTickableWhenPaused() const override
  {
    return false; // Decide if it should tick when the game is paused
  }

  virtual bool IsTickableInEditor() const override
  {
    return false; // Typically you don't need it to tick in editor, but you can change this
  }

private:
  TFunction<void()> Callback;
  float HoldInterval;
  float AccumulatedTime;
};

UCLASS()
class WIDGETSTOOLKIT_API UButtonBaseWidget : public UWidgetBase
{
  GENERATED_BODY()

public:
  virtual void NativeConstruct() override;
  virtual void SynchronizeProperties() override;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button Base", meta = (BindWidget))
  TObjectPtr<UButton> Button;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Base", meta = (DisplayName = "Button Style"))
  FButtonStyle ButtonStyle;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Base")
  bool bUseOnHoldEvent = false;

  /* Use this event to fire instructions while the button is held. */
  FOnButtonHoldSignature OnHoldEvent;

  /* If the OnHoldEvent is bound this timer will be used to measure how much time must pass before the event is fired. BE WARNED: this is
   * not about event AFTER the button was pressed, but WHILE it is pressed. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Base")
  float OnHoldInterval = 0.05f;

protected:
  /* These methods add and remove tick callback to continuously pressed button. */
  TUniquePtr<FButtonHoldTicker> HoldTicker;

  UFUNCTION() void OnPressedDefault();
  UFUNCTION() void OnReleasedDefault();

  /* Often buttons are used to open close or otherwise controll other widgets. This allows to set the reference for that widget. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TObjectPtr<UWidgetBase> ControlledWidget;

public:
  UFUNCTION(BlueprintCallable)
  void SetControlledWidget(UWidgetBase* Widget);
};
