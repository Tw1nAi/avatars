// Juice Sp. z o. o.. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "Log.generated.h"

UCLASS(NotBlueprintable)
/**
 * The base class for all Unreal Engine objects.
 */
class AVATARS_API ULog final : public UObject
{
  GENERATED_BODY()

public:
  /**
   * Displays an informational message.
   *
   * @param Message The message to display.
   * @param Duration The duration in seconds for which the message should be displayed. Default is 10 seconds.
   */
  static void Info(const FString Message, const float Duration = 10.f);

  /**
   * Displays an informational message if a condition is true.
   *
   * @param bCondition The condition to check.
   * @param Message The message to display.
   * @param Duration The duration in seconds for which the message should be displayed. Default is 10 seconds.
   * @return True if the message was displayed, false otherwise.
   */
  static bool InfoIf(const bool bCondition, const FString Message, const float Duration = 10.f);

  /**
   * Displays a warning message.
   *
   * @param Message The message to display.
   * @param Duration The duration in seconds for which the message should be displayed. Default is 10 seconds.
   */
  static void Warn(const FString Message, const float Duration = 10.f);

  /**
   * Displays a warning message if a condition is true.
   *
   * @param bCondition The condition to check.
   * @param Message The message to display.
   * @param Duration The duration in seconds for which the message should be displayed. Default is 10 seconds.
   * @return True if the warning was thrown, false otherwise.
   */
  static bool WarnIf(const bool bCondition, const FString Message, const float Duration = 10.f);

  /**
   * Displays an error message.
   *
   * @param ErrorMessage The error message to display.
   * @param bLogStack Whether to log the call stack along with the error message. Default is false.
   * @param Duration The duration in seconds for which the message should be displayed. Default is 10 seconds.
   */
  static void Error(const FString ErrorMessage, const bool bLogStack = false, const float Duration = 10.f);

  /**
   * Displays an error message if a condition is true.
   *
   * @param bCondition The condition to check.
   * @param ErrorMessage The error message to display.
   * @param bLogStack Whether to log the call stack along with the error message. Default is false.
   * @param Duration The duration in seconds for which the message should be displayed. Default is 10 seconds.
   * @return True if the error was thrown, false otherwise.
   */
  static bool ErrorIf(const bool bCondition, const FString ErrorMessage, const bool bLogStack = false, const float Duration = 10.f);
};