// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "ButtonBaseWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "CoreMinimal.h"

#include "OptionsMenu.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FPanelSettings
{
  GENERATED_BODY()

  /* Displayed at the panel header. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FText CategoryName;

  /* Used for buttons text and lables. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FText Label;

  /* The panel widget class. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TSubclassOf<UWidgetBase> PanelWidgetClass;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TSubclassOf<UButtonBaseWidget> ButtonWidgetClass;
};

UCLASS()
class AVATARS_API UOptionsMenu : public UWidgetBase
{
  GENERATED_BODY()
public:
  virtual void ShowNative() override;
  virtual void HideNative() override;

  virtual void NativeConstruct() override;
  virtual void SynchronizeProperties() override;

  void BuildPanels();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options Panel")
  TArray<FPanelSettings> OptionsPanels;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options Panel")
  float MenuItemsSpacing = 5.0f;
  UFUNCTION()
  void HideAllPanels();
  UFUNCTION()
  void HideAllExceptOf(UWidget* WidgetToKeepVisible);

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Options Panel")
  TObjectPtr<UVerticalBox> MenuContainer;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Options Panel")
  TObjectPtr<UVerticalBox> OptionsContainer;

  /* Timeout configuration */
  /* ********************************************************** */
  /* Thie idea is that options menu should be accessible to service stuff only.
  To avoid situations where menu is accesed
  by users we set timer that will close the menu after set seconds. */

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options Panel")
  bool bUseTimeout = true;

  UFUNCTION(BlueprintCallable, Category = "Options Panel")
  void SetUseTimeout(bool bUse);

  void ClearTimeout();

  /* The menu should be accessible to service stuff only, and once opened it should automatically close after short time to prevent it from
   * being visible to users. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options Panel")
  float MenuTimeout = 30.0f;

  FTimerHandle MenuTimeoutHandle;

  /* Use this method to start/restart the menu service timeout. */
  UFUNCTION(BlueprintCallable, Category = "Options Panel")
  void StartMenuTimeout(const float Timeout = 30.0f);

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Options Panel")
  UTextBlock* TimoutDisplay;

  UPROPERTY()
  float TimoutCounter;

  UFUNCTION()
  void OnTimeoutTick();
  void UpdateTimeoutDisplay();
  FTimerHandle TimeoutTickHandle;
};
