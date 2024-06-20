#pragma once

#include "CoreMinimal.h"

#include "TextHelpers.generated.h"

USTRUCT()
struct FTextHelpers
{
  GENERATED_BODY()
  /**
   * Calculates the reading time for a given input text.
   *
   * @param InputText The text to calculate the reading time for.
   * @param WordsPerSecond The average number of words read per second. Default value is 3.33.
   * @return The reading time in seconds.
   */
  static float CalculateReadingTime(const FString& InputText, const float WordsPerSecond = 3.33f)
  {
    // Count words in the input string
    int32 WordCount = CountWords(InputText);

    // Calculate the delay in seconds
    float DelayInSeconds = WordCount / WordsPerSecond;

    return DelayInSeconds;
  }

  /**
   * Counts the number of words in a given text.
   *
   * @param InputText The text to count the words in.
   * @return The number of words in the text.
   */
  static int32 CountWords(const FString& InputText)
  {
    int32 WordCount = 0;
    bool bInWord = false;

    for (const TCHAR& Character : InputText)
    {
      if (FChar::IsWhitespace(Character))
      {
        if (bInWord)
        {
          bInWord = false;
        }
      }
      else
      {
        if (!bInWord)
        {
          bInWord = true;
          ++WordCount;
        }
      }
    }

    return WordCount;
  }
};
