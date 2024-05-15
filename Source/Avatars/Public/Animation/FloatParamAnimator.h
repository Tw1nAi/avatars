// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "FloatParamAnimator.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FFloatAnimationSettings
{
  GENERATED_BODY()

  /* The curve that drives the float animation. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UCurveFloat* AnimationCurve;

  /**
   * Time range used to randomly choose the duration for the float animation to randomize its duration.
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector2D SpeedRange = FVector2D(0.0f, 0.0f);

  /**
   * Time range used to randomly choose how long to wait before next animation.
   * The default values seems to be ok for humans in idle mode.
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector2D AnimationRange = FVector2D(0.0f, 0.0f);
};

/**
 * Simple tool to animate float parameters at time intervals.
 */
UCLASS()
class AVATARS_API UFloatParamAnimator : public UObject
{
  GENERATED_BODY()

public:
  /* Enable alpha animation with the give settings. */
  UFUNCTION(BlueprintCallable)
  void PlayAnimation();

  UFUNCTION(BlueprintCallable)
  void TickAnimation(const float DeltaTime);

  UFUNCTION(BlueprintCallable)
  float GetAlpha() const { return Alpha; }

  /* Call that after creating instance of this class to set your own settings. */
  UFUNCTION(BlueprintCallable)
  void SetAnimationSettings(const FFloatAnimationSettings& Settings) { AnimationSettings = Settings; }

private:
  /* Value from in range zero to one that can be used to drive animation.  */
  UPROPERTY()
  float Alpha;

  /* The speed of the animation. */
  UPROPERTY()
  float Speed;

  /* Current curve time for the eyes close animation. */
  UPROPERTY()
  float AnimationCurveTime;

  float GetSpeed() const;
  float GetNextTime() const;

  FTimerHandle TimerHandle;

  UPROPERTY()
  FFloatAnimationSettings AnimationSettings;
};
