#pragma once

#include "CoreMinimal.h"

#include "TextHelpers.generated.h"

USTRUCT()
struct FTextHelpers
{
  GENERATED_BODY()
  // Function to calculate Levenshtein distance between two FString inputs
  static int32 LevenshteinDistance(const FString& Source, const FString& Target)
  {
    const int32 SourceLength = Source.Len();
    const int32 TargetLength = Target.Len();

    // Early exit for trivial cases
    if (SourceLength == 0) return TargetLength;
    if (TargetLength == 0) return SourceLength;

    // Create a 2D array to store distances
    TArray<TArray<int32>> DistanceMatrix;
    DistanceMatrix.SetNum(SourceLength + 1);
    for (int32 i = 0; i <= SourceLength; i++)
    {
      DistanceMatrix[i].SetNum(TargetLength + 1);
    }

    // Initialize the matrix
    for (int32 i = 0; i <= SourceLength; i++)
    {
      DistanceMatrix[i][0] = i;
    }

    for (int32 j = 0; j <= TargetLength; j++)
    {
      DistanceMatrix[0][j] = j;
    }

    // Fill the matrix
    for (int32 i = 1; i <= SourceLength; i++)
    {
      for (int32 j = 1; j <= TargetLength; j++)
      {
        // Check if characters are equal (case-sensitive comparison)
        int32 Cost = (Source[i - 1] == Target[j - 1]) ? 0 : 1;

        // Calculate the minimum cost
        DistanceMatrix[i][j] = FMath::Min3(
            DistanceMatrix[i - 1][j] + 1,       // Deletion
            DistanceMatrix[i][j - 1] + 1,       // Insertion
            DistanceMatrix[i - 1][j - 1] + Cost // Substitution
        );
      }
    }

    // The final Levenshtein distance is found in the bottom-right cell
    return DistanceMatrix[SourceLength][TargetLength];
  }
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
