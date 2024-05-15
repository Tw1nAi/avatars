// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"

#include "DebounceGuard.generated.h"

USTRUCT()
struct FDebounceGuard
{
  GENERATED_BODY()

private:
  FTimerHandle TimerHandle;
  float Delay;
  bool bIsLocked;

  // Function to unlock
  void Unlock() { bIsLocked = false; }

public:
  // Constructor
  explicit FDebounceGuard(float InDelay = 0.5) : Delay(InDelay), bIsLocked(false) {}

  // Functor operator to check lock status
  bool operator()(UWorld* World)
  {
    if (!World) return false;

    if (bIsLocked)
    {
      // Still locked, so return true to indicate locking is active
      return true;
    }

    // Lock and set a timer to unlock after the delay
    bIsLocked = true;
    World->GetTimerManager().SetTimer(
        TimerHandle,
        FTimerDelegate::CreateLambda([&] {
          if (this) this->Unlock();
        }),
        Delay,
        false
    );

    // Return false indicating that locking just started, allowing operation this time
    return false;
  }
};
