// Copyright Juice sp. z o. o., All Rights Reserved.

#include "Components/TextInputFiltered.h"

#include "Runtime/Core/Public/Internationalization/Regex.h"

UTextInputFiltered::UTextInputFiltered()
{
  // Set default filter mode and white space filter option
  TextFilterMode = ETextFilterMode::NoFiltering;
  bFilterOutWhiteSpaces = false;

  // Bind the text change event
  OnTextChanged.AddDynamic(this, &UTextInputFiltered::CustomOnTextChanged);
}

void UTextInputFiltered::CustomOnTextChanged(const FText& InText)
{
  FString FilteredText = ApplyFilter(InText.ToString());

  // Update the text with the filtered result (avoids triggering the event again)
  SetText(FText::FromString(FilteredText));
}

FString UTextInputFiltered::ApplyFilter(const FString& InputText)
{
  FString Result;

  // Apply the selected filter mode
  switch (TextFilterMode)
  {
  case ETextFilterMode::LettersOnly:
    for (TCHAR Char : InputText)
    {
      if (FChar::IsAlpha(Char) || FChar::IsWhitespace(Char))
      {
        Result.AppendChar(Char);
      }
    }
    break;

  case ETextFilterMode::DigitsOnly:
    for (TCHAR Char : InputText)
    {
      if (FChar::IsDigit(Char) || FChar::IsWhitespace(Char))
      {
        Result.AppendChar(Char);
      }
    }
    break;

  case ETextFilterMode::LettersAndDigits:
    for (TCHAR Char : InputText)
    {
      if (FChar::IsAlnum(Char) || FChar::IsWhitespace(Char))
      {
        Result.AppendChar(Char);
      }
    }
    break;

  case ETextFilterMode::CustomRegex:
    if (!CustomRegexPattern.IsEmpty())
    {
      Result = ApplyCustomRegex(InputText, CustomRegexPattern);
    }
    break;

  case ETextFilterMode::NoFiltering:
  default:
    break;
  }

  // Optionally filter out white spaces
  if (bFilterOutWhiteSpaces)
  {
    Result = Result.Replace(TEXT(" "), TEXT(""));
  }

  return Result;
}

FString UTextInputFiltered::ApplyCustomRegex(const FString& InputText, const FString& RegexPattern)
{
  FRegexPattern Pattern(RegexPattern);
  FRegexMatcher Matcher(Pattern, InputText);

  FString FilteredText;

  // Iterate over the matches and build the filtered string
  while (Matcher.FindNext())
  {
    FilteredText += Matcher.GetCaptureGroup(0);
  }

  return FilteredText;
}
