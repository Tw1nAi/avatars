// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/FloatParamAnimator.h"
#include "CoreMinimal.h"
#include "Math/Vector2D.h"

#include "FaceAnimInstance.generated.h"

class UCurveFloat;

USTRUCT(BlueprintType, Blueprintable)
struct FBlinkAnimationSettings
{
  GENERATED_BODY()

  /* The curve that drives the close and open animation of the eyes. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UCurveFloat* BlinkAnimationCurve;

  /**
   * Time range used to randomly choose the duration for the blink to randomize blink duration.
   * The default values are more suitable for humans and human sized creatures.
   * Use shorter time for smaller creature and longer for bigger creatures.
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector2D BlinkSpeedRange = FVector2D(0.2f, 0.35f);

  /**
   * Time range used to randomly choose how long to wait before next blink animation.
   * The default values seems to be ok for humans in idle mode.
   */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector2D BlinkAnimationRange = FVector2D(5.0f, 7.0f);
};

UCLASS()
class AVATARS_API UFaceAnimInstance : public UAnimInstance
{
  GENERATED_BODY()

public:
  UFaceAnimInstance();
  virtual void NativeBeginPlay() override;
  virtual void NativeUpdateAnimation(float DeltaSeconds) override;

  UFUNCTION(BlueprintCallable)
  void PlayFaceAnimation(UAnimSequenceBase* Animation);

  UFUNCTION(BlueprintCallable)
  void StopFaceAnimation();

  /* Toggle this on and off to see runtime properties in the details panel. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bShowRuntimeProperties;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UAnimSequenceBase* DialogAnimation;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bIsFaceAnimationValid;

  /* Blink Animation */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eyes Animation")
  FFloatAnimationSettings BlinkAnimationSettings;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Eyes Animation")
  bool bPlayBlinkAnimation;

  UPROPERTY(
      EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides), Category = "Eyes Animation"
  )
  UFloatParamAnimator* BlinkAnimation;

  UPROPERTY(
      EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides), Category = "Eyes Animation"
  )
  float BlinkAlpha;

  /* Left Ear Animation */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ears Animation")
  FFloatAnimationSettings LeftEarAnimationSettings;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ears Animation")
  bool bPlayLeftEarAnimation;

  UPROPERTY(
      EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides), Category = "Ears Animation"
  )
  UFloatParamAnimator* LeftEarAnimation;

  UPROPERTY(
      EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides), Category = "Ears Animation"
  )
  float LeftEarAlpha;

  /* Right Ear Animation */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ears Animation")
  FFloatAnimationSettings RightEarAnimationSettings;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ears Animation")
  bool bPlayRightEarAnimation;

  UPROPERTY(
      EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides), Category = "Ears Animation"
  )
  UFloatParamAnimator* RightEarAnimation;

  UPROPERTY(
      EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides), Category = "Ears Animation"
  )
  float RightEarAlpha;
};
