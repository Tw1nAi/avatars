// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

// Engine
#include "Components/EditableText.h"
#include "CoreMinimal.h"

#include "TextInputFiltered.generated.h"

UENUM(BlueprintType)
enum class ETextFilterMode : uint8
{
  NoFiltering UMETA(DisplayName = "No filtering"),
  LettersOnly UMETA(DisplayName = "Filter out all non-letter characters"),
  DigitsOnly UMETA(DisplayName = "Filter out all non-digit characters"),
  LettersAndDigits UMETA(DisplayName = "Filter out all non-letter and non-digit characters"),
  CustomRegex UMETA(DisplayName = "Custom regular expression filter")
};

UCLASS()
class WIDGETSTOOLKIT_API UTextInputFiltered : public UEditableText
{
  GENERATED_BODY()

public:
  UTextInputFiltered();

  /** Filter mode to apply on the text on each input. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextFilter")
  ETextFilterMode TextFilterMode;

  /** Custom regular expression filter. Will be applied if the TextFilterMode is set as CustomRegex only. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextFilter")
  FString CustomRegexPattern;

  /** Whether to filter out white spaces */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextFilter")
  bool bFilterOutWhiteSpaces;

protected:
  // Called when the text is changed
  UFUNCTION()
  void CustomOnTextChanged(const FText& InText);

private:
  // Filters the input text based on the selected filter mode
  FString ApplyFilter(const FString& InputText);

  // Helper function for regex filtering
  FString ApplyCustomRegex(const FString& InputText, const FString& RegexPattern);
};
