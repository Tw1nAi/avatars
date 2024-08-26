#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "WidgetBase.generated.h"

UCLASS()
class WIDGETSTOOLKIT_API UWidgetBase : public UUserWidget
{
  GENERATED_BODY()

public:
  /* Visibility and interaction utility methods */
  UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIsInteractive = false;
  UFUNCTION(BlueprintCallable) void SetInteractive(const bool bInteractive, const bool bUpdateVisibility = true);
  UFUNCTION(BlueprintCallable, BlueprintPure) bool IsInteractive() const;
  UFUNCTION(BlueprintCallable) void Show();
  UFUNCTION(BlueprintCallable) void ShowNotInteractive(const bool bAllowChildreInteractive = true);
  UFUNCTION(BlueprintCallable) void Hide(const bool bShouldCollapse = true);
  UFUNCTION(BlueprintCallable, BlueprintPure) bool IsCollapsed() const;
  UFUNCTION(BlueprintCallable, BlueprintPure) bool IsHidden() const;
};