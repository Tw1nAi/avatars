// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/CircularThrobber.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"

#include "LoadingScreenWidget.generated.h"

UCLASS()
class AVATARS_API ULoadingScreenWidget : public UUserWidget
{
  GENERATED_BODY()

public:
  void Hide();
  void Show(const FString NewLoadingText = "", const bool bShowSpinner = true);

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  UWidgetAnimation* HideAnimation;

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  UWidgetAnimation* ShowAnimation;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
  UTextBlock* LoadingText;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
  UCircularThrobber* Spinner;

  UFUNCTION()
  void OnAnimHideFinished();

protected:
  virtual void NativeConstruct() override;
  FWidgetAnimationDynamicEvent OnAnimHideDelegate;
};
