// Copyright Juice sp. z o. o., All Rights Reserved.

#include "Animation/FloatParamAnimator.h"

#include "Get.h"
#include "Log.h"

void UFloatParamAnimator::PlayAnimation()
{
  // Set the curve time to 0.0f to start the animation from the beginning.
  AnimationCurveTime = 0.0f;
  Alpha = 0.0f;
  Speed = GetSpeed();

  GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UFloatParamAnimator::PlayAnimation, GetNextTime(), false);
}

float UFloatParamAnimator::GetSpeed() const
{
  return FMath::RandRange(AnimationSettings.SpeedRange.X, AnimationSettings.SpeedRange.Y);
}

float UFloatParamAnimator::GetNextTime() const
{
  return FMath::RandRange(AnimationSettings.AnimationRange.X, AnimationSettings.AnimationRange.Y);
}

void UFloatParamAnimator::TickAnimation(const float DeltaTime)
{
  if (ULog::ErrorIf(AnimationSettings.AnimationCurve == nullptr, "AnimationCurve in blink animation settings is not set.")) return;

  AnimationCurveTime += DeltaTime / Speed;
  Alpha = FMath::Clamp(AnimationSettings.AnimationCurve->GetFloatValue(AnimationCurveTime), 0.0f, 1.0f);
}
