// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Log.h"
#include "TimerManager.h"

#include "TimeoutManager.generated.h"

USTRUCT(BlueprintType)
struct FTimeoutManager
{
  GENERATED_BODY()

  FTimerHandle TimerHandle;

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bDebug;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float TimeoutDelay = 0.0f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bUseTimeout = false;

  FString Name;

  FTimeoutManager(float Delay = 0.0f, bool bTimeout = false, const FString InName = FString("TimeoutManager"))
      : TimeoutDelay(Delay), bUseTimeout(bTimeout), Name(InName){};

  void SetUseTimeout(const bool bInUseTimeout)
  {
    if (bDebug) ULog::Info("Setting use timeout: " + Name + " to " + FString::FromInt(bInUseTimeout));
    bUseTimeout = bInUseTimeout;
  }

  void SetTimeoutDelay(const float NewTimeout)
  {
    if (bDebug) ULog::Info("Setting timeout: " + Name + " to " + FString::SanitizeFloat(NewTimeout));
    TimeoutDelay = NewTimeout;
  }

  bool IsReady() { return bUseTimeout && TimeoutDelay > 0.0f; }

  bool IsValid() { return TimerHandle.IsValid(); }

  void ClearIfValid(UWorld* World)
  {
    if (TimerHandle.IsValid() && World != nullptr)
    {
      if (bDebug) ULog::Info("Clearing timer: " + Name);
      World->GetTimerManager().ClearTimer(TimerHandle);
      TimerHandle = FTimerHandle();
    }
  }

  FText GetDelayAsText() { return FText::FromString(FString::SanitizeFloat(TimeoutDelay)); }
};