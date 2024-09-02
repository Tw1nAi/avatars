// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SelectionOption.generated.h"

/**
 * Universal selection option structure.
 */
USTRUCT(BlueprintType, Blueprintable)
struct FSelectionOption
{
  GENERATED_BODY()

  // clang-format off
  FSelectionOption(
    const FText InLabel = FText::GetEmpty(),
    const int32 InValue = 0,
    const float InFloatValue = 0.0f,
    const FString InStringValue = ""
  )
      : Label(InLabel),
        IntValue(InValue),
        FloatValue(InFloatValue),
        StringValue(InStringValue)
        {};
  // clang-format on

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
  FText Label;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
  int32 IntValue;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
  float FloatValue;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Option")
  FString StringValue;

  FSelectionOption GetCopy() const
  {
    FSelectionOption Copy;
    Copy.Label = Label;
    Copy.IntValue = IntValue;
    Copy.FloatValue = FloatValue;
    Copy.StringValue = FString(StringValue);

    return Copy;
  }
};