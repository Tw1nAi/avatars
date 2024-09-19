// Juice Sp. z o. o.. All rights reserved.

#include "Log.h"

#include "Engine/Engine.h"
#include "Misc/OutputDeviceDebug.h"

void ULog::Info(const FString Message, const float Duration)
{
  // GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::White, Message);
  UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
}

bool ULog::InfoIf(const bool bCondition, const FString Message, const float Duration)
{
  if (!bCondition) return false;

  // // GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::White, Message);
  UE_LOG(LogTemp, Display, TEXT("%s"), *Message);

  return true;
}

void ULog::Warn(const FString Message, const float Duration)
{
  // // GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Yellow, Message);
  UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

bool ULog::WarnIf(const bool bCondition, const FString Message, const float Duration)
{
  if (!bCondition) return false;

  // // GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Yellow, Message);
  UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

  return true;
}

void ULog::Error(const FString ErrorMessage, const bool bLogStack, const float Duration)
{
  // // GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Red, ErrorMessage);
  UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
  if (bLogStack) FDebug::DumpStackTraceToLog(ELogVerbosity::Type::Error);
}

bool ULog::ErrorIf(const bool bCondition, const FString ErrorMessage, const bool bLogStack, const float Duration)
{
  if (!bCondition) return false;

  // // GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Red, ErrorMessage);
  UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
  if (bLogStack) FDebug::DumpStackTraceToLog(ELogVerbosity::Type::Error);

  return true;
}
