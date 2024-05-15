// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Components/Slider.h"
#include "CoreMinimal.h"
#include "UserInterface/BaseWidget.h"
#include "UserInterface/ButtonBaseWidget.h"
#include "UserInterface/TextButtonWidget.h"

#include "CameraOptionsWidget.generated.h"

UCLASS()
class AVATARS_API UCameraOptionsWidget : public UBaseWidget
{
  GENERATED_BODY()

  void OnPositionButtonClicked(FVector2D Direction);

  UFUNCTION() void Center();
  UFUNCTION() void MoveUp();
  UFUNCTION() void MoveRight();
  UFUNCTION() void MoveDown();
  UFUNCTION() void MoveLeft();
  UFUNCTION() void OnZoomSliderValueChanged(float Value);

public:
  virtual void NativeConstruct() override;
  virtual void SynchronizeProperties() override;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UTextButtonWidget* CenterButton;
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButtonBaseWidget* TopButton;
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButtonBaseWidget* RightButton;
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButtonBaseWidget* DownButton;
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButtonBaseWidget* LeftButton;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  USlider* ZoomSlider;

  UFUNCTION(BlueprintCallable)
  void SetZoom(const float Value);

  void ResetZoom();
  void SaveSettings();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float ZoomSpeed = 50.0f;
};
