// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "BaseWidget.generated.h"

UCLASS()
class AVATARS_API UBaseWidget : public UUserWidget
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable)
  void Show();
  virtual void ShowNative();

  UFUNCTION(BlueprintCallable)
  void Hide();
  virtual void HideNative();
};
