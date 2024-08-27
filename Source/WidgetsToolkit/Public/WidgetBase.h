#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "WidgetBase.generated.h"

/**
 * To simplify the process of creating interactive and non-interactive widgets with this class use
 * Show() and Hide() methods instead of SetVisibility() to make the code more readable
 * and Toggle() to simplify the process of toggling the widget visibility.
 * The bIsInteractive flag is used to determine if the widget should be interactive or not.
 */
UCLASS()
class WIDGETSTOOLKIT_API UWidgetBase : public UUserWidget
{
  GENERATED_BODY()

public:
  virtual void NativeConstruct() override;
  /* Visibility and interaction utility methods */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Base")
  bool bIsInteractive = false;

  UFUNCTION(BlueprintCallable, Category = "Widget Base")
  void SetInteractive(const bool bInteractive, const bool bUpdateVisibility = true);

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Widget Base")
  bool IsInteractive() const;

  UFUNCTION(BlueprintCallable, Category = "Widget Base")
  void Show();
  UFUNCTION(BlueprintCallable, Category = "Widget Base")
  void ShowDelayed();
  virtual void ShowNative();

  UFUNCTION(BlueprintCallable, Category = "Widget Base")
  void ShowNotInteractive(const bool bAllowChildrenInteractive = true);

  UFUNCTION(BlueprintCallable, Category = "Widget Base")
  void Hide(const bool bShouldCollapse = true);
  virtual void HideNative();

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Widget Base")
  bool IsCollapsed() const;

  UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Widget Base")
  bool IsHidden() const;

  /* Use this instead of regular visibility methods to simplify widget show and hide actions. */
  UFUNCTION(BlueprintCallable, Category = "Widget Base")
  void Toggle();

  UFUNCTION(BlueprintCallable, Category = "Widget Base")
  void SetToggle(const bool bToggle);

  /* If set to above 0 (zero) the delay will be added before the toggle will be performed. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Base")
  float ToggleDelay = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget Base")
  bool bIsToggled = true;
};