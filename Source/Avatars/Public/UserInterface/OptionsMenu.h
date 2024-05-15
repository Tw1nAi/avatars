// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "CoreMinimal.h"
#include "UserInterface/BaseWidget.h"
#include "UserInterface/CameraOptionsWidget.h"
#include "UserInterface/ConversationOptionsWidget.h"

#include "OptionsMenu.generated.h"

UCLASS()
class AVATARS_API UOptionsMenu : public UBaseWidget
{
  GENERATED_BODY()
public:
  virtual void ShowNative() override;
  virtual void HideNative() override;

  virtual void NativeConstruct() override;

  void HideAllExceptOf(UWidget* WidgetToKeepVisible);

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UVerticalBox* OptionsContainer;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButton* OpenConversationMenuButton;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UConversationOptionsWidget* ConversationOptions;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButton* OpenCameraOptionsButton;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UCameraOptionsWidget* CameraOptions;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButton* CloseOptionsButton;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UButton* CloseAppButton;

  UFUNCTION()
  void OnOpenConversationMenuButtonClicked();

  UFUNCTION()
  void OnOpenCameraOptionsButtonClicked();

  /* Timeout configuration */
  /* ********************************************************** */
  /* Thie idea is that options menu should be accessible to service stuff only. To avoid situations where menu is accesed
  by users we set timer that will close the menu after set seconds. */

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bUseTimeout = true;

  UFUNCTION(BlueprintCallable)
  void SetUseTimeout(bool bUse);

  void ClearTimeout();

  /* The menu should be accessible to service stuff only, and once opened it should automatically close after short time to prevent it from
   * being visible to users. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars Player Controller|Service")
  float MenuTimeout = 30.0f;

  FTimerHandle MenuTimeoutHandle;

  /* Use this method to start/restart the menu service timeout. */
  UFUNCTION(BlueprintCallable)
  void StartMenuTimeout(const float Timeout = 30.0f);

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UTextBlock* TimoutDisplay;

  UPROPERTY()
  float TimoutCounter;

  UFUNCTION()
  void OnTimeoutTick();
  void UpdateTimeoutDisplay();
  FTimerHandle TimeoutTickHandle;
};
