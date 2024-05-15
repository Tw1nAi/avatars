// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TimerManager.h"

#include "Debouncer.generated.h"

USTRUCT()
struct FDebouncer
{
  GENERATED_BODY()

private:
  TFunction<void()> DebouncedFunction;
  FTimerHandle TimerHandle;
  float Delay;

  void TriggerFunction()
  {
    if (DebouncedFunction) DebouncedFunction();
  }

public:
  // Constructor to initialize the debouncer with a delay
  FDebouncer(float InDelay = 0.5) : Delay(InDelay) {}

  // Functor operator to set or reset the debounced function
  void operator()(TFunction<void()> InFunction, UWorld* World)
  {
    if (World == nullptr) return;

    // Store the function and the world context
    DebouncedFunction = InFunction;

    // Reset the timer
    World->GetTimerManager().ClearTimer(TimerHandle);
    TimerHandle = FTimerHandle();

    // Set the timer to call the function after the specified delay
    World->GetTimerManager().SetTimer(
        TimerHandle,
        FTimerDelegate::CreateLambda([&] {
          if (this) this->TriggerFunction();
        }),
        Delay,
        false
    );
  }
};