// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/TextBlock.h"
#include "SelectionOption.h"
#include "WidgetBase.h"

#include "SelectionWidgetBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogButtonSelectionWidget, Log, All);

/**
 * Selection widget base that uses two buttons - left and right - to navigate through the provided options.
 */
UCLASS(BlueprintType, Blueprintable)
class WIDGETSTOOLKIT_API USelectionWidgetBase : public UWidgetBase
{
  GENERATED_BODY()
public:
  USelectionWidgetBase();
  virtual void NativeConstruct() override;
  // virtual UWidget* NativeGetDesiredFocusTarget() const override;

  void Clear();
  void AddOption(const FSelectionOption& Option);
  void SetCurrentSelection(const int32 Index);

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString OptionName;

  UFUNCTION(BlueprintCallable)
  void SelectPrevious();

  UFUNCTION(BlueprintCallable)
  void SelectNext();

  DECLARE_DELEGATE_OneParam(FOnSelectionChanged, int32);
  FOnSelectionChanged OnSelectionChanged;

  UFUNCTION(BlueprintCallable)
  FSelectionOption& GetSelectedOption();

  /**
   * Call this function after every option change or when new set of options are applied.
   * Mind, that if during NativeConstruct there are no options and they are added after
   * creating widget you will have to call this method manually afterwards.
   * */
  UFUNCTION(BlueprintCallable)
  void UpdateSelectionText();

protected:
  UFUNCTION()
  UWidget* OnNavigation(EUINavigation InNavigation);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Selection Widget Base")
  TArray<FSelectionOption> Options;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget), Category = "Selection Widget Base")
  TObjectPtr<UTextBlock> LabelWidget;

  int SelectedOptionIndex = 0;
};
