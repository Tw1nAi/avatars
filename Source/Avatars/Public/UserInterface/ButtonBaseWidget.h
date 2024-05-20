// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Components/Button.h"
#include "Components/NamedSlot.h"
#include "CoreMinimal.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Tickable.h"
#include "UserInterface/BaseWidget.h"

#include "ButtonBaseWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonHoldSignature);

class FButtonHoldTicker : public FTickableGameObject
{
public:
  FButtonHoldTicker(std::function<void()> InCallback, float InHoldInterval)
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
  std::function<void()> Callback;
  float HoldInterval;
  float AccumulatedTime;
};

UCLASS()
class AVATARS_API UButtonBaseWidget : public UBaseWidget
{
  GENERATED_BODY()

public:
  virtual void NativeConstruct() override;
  virtual void SynchronizeProperties() override;

  UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
  UButton* Button;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Image Button", meta = (DisplayName = "Button Style"))
  FButtonStyle ButtonStyle;

  /* Use this event to fire instructions while the button is held. */
  FOnButtonHoldSignature OnHoldEvent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float OnHoldInterval = 0.05f;

protected:
  /* These methods add and remove tick callback to continuously pressed button. */
  TUniquePtr<FButtonHoldTicker> HoldTicker;

private:
  UFUNCTION() void OnPressedDefault();
  UFUNCTION() void OnReleasedDefault();
};
