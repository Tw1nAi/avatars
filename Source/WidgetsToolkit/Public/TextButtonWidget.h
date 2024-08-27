// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "ButtonBaseWidget.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"

#include "TextButtonWidget.generated.h"

UCLASS()
class WIDGETSTOOLKIT_API UTextButtonWidget : public UButtonBaseWidget
{
  GENERATED_BODY()

public:
  virtual void SynchronizeProperties() override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text Button")
  FText Text;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UTextBlock* TextBlock;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text Button")
  FSlateFontInfo Font;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text Button")
  FSlateColor TextColor;

  UFUNCTION(BlueprintCallable)
  void SetText(const FText& NewText);
};
