// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Get.h"
#include "Log.h"
#include "Misc/DefaultValueHelper.h"
#include "TimerManager.h"

#include "TextUtils.generated.h"

/* Utility class to help with user inputs. */
UCLASS()
class UTextUtils : public UObject

{
  GENERATED_BODY()

public:
  /**
   * Converts a given FText to a float value.
   *
   * @param InText The input FText to convert.
   * @param OutFloat The converted float value will be stored in this variable.
   * @param Sign The sign of the converted float value (-1 for negative, 1 for positive). Effectively clamps output to positive or negative
   * values.
   * @param OutString Optional output parameter to store the filtered string after removing non-digit characters.
   * @return True if the conversion was successful, false otherwise.
   */
  static FORCEINLINE bool TextToFloat(const FText& InText, float& OutFloat, const int32 Sign = 0, FString* OutString = nullptr)
  {
    if (InText.IsEmpty())
    {
      return false;
    }

    FString OriginalString = InText.ToString().TrimStartAndEnd();

    int32 FirstDotIndex = OriginalString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromStart);
    if (FirstDotIndex != INDEX_NONE)
    {
      int32 SecondDotIndex = OriginalString.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromStart, FirstDotIndex + 1);
      if (SecondDotIndex != FirstDotIndex && SecondDotIndex != INDEX_NONE)
      {
        OriginalString = OriginalString.Left(SecondDotIndex);
      }
    }

    FString FilteredString;

    // Iterate through each character in the original string
    for (TCHAR Char : OriginalString)
    {
      // Check if the character is a digit
      if (FChar::IsDigit(Char) || Char == *".")
      {
        FilteredString.AppendChar(Char);
      }
    }

    /* If text starts with . prepend a zero. */
    if (!FilteredString.IsEmpty() && FilteredString[0] == *".")
    {
      FilteredString = TEXT("0") + FilteredString;
    }

    if (OutString != nullptr)
    {
      *OutString = FilteredString;
    }

    float Delay = 0.f;
    // check if text input has any digits and can be vonverted to flaot
    if (!FDefaultValueHelper::ParseFloat(FilteredString, Delay))
    {
      ULog::Error("Could not parse text input into float");
      return false;
    }

    if (Sign == -1)
    {
      Delay = FMath::Abs(Delay) * -1;
    }
    else if (Sign == 1)
    {
      Delay = FMath::Abs(Delay);
    }

    OutFloat = Delay;
    return true;
  }
};
