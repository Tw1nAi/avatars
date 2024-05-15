// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "UserInterface/ButtonBaseWidget.h"

#include "TextButtonWidget.generated.h"

UCLASS()
class AVATARS_API UTextButtonWidget : public UButtonBaseWidget
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
};
