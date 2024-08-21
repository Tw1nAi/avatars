// Copyright Juice sp. z o. o., All Rights Reserved.

#include "FaceAnimInstance.h"

#include "Curves/CurveFloat.h"
#include "Get.h"
#include "Log.h"

const FName DefaultSlotName = FName("DefaultSlot");

UFaceAnimInstance::UFaceAnimInstance()
{
  bPlayBlinkAnimation = true;
}

void UFaceAnimInstance::NativeBeginPlay()
{
  BlinkAnimation = NewObject<UFloatParamAnimator>(this, UFloatParamAnimator::StaticClass());
  BlinkAnimation->SetAnimationSettings(BlinkAnimationSettings);
  if (bPlayBlinkAnimation) BlinkAnimation->PlayAnimation();

  LeftEarAnimation = NewObject<UFloatParamAnimator>(this, UFloatParamAnimator::StaticClass());
  LeftEarAnimation->SetAnimationSettings(LeftEarAnimationSettings);
  if (bPlayLeftEarAnimation) LeftEarAnimation->PlayAnimation();

  RightEarAnimation = NewObject<UFloatParamAnimator>(this, UFloatParamAnimator::StaticClass());
  RightEarAnimation->SetAnimationSettings(RightEarAnimationSettings);
  if (bPlayRightEarAnimation) RightEarAnimation->PlayAnimation();
}

void UFaceAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
  if (bPlayBlinkAnimation && BlinkAnimation != nullptr)
  {
    BlinkAnimation->TickAnimation(DeltaSeconds);
    BlinkAlpha = BlinkAnimation->GetAlpha();
  }

  if (bPlayLeftEarAnimation && LeftEarAnimation != nullptr)
  {
    LeftEarAnimation->TickAnimation(DeltaSeconds);
    LeftEarAlpha = LeftEarAnimation->GetAlpha();
  }

  if (bPlayRightEarAnimation && RightEarAnimation != nullptr)
  {
    RightEarAnimation->TickAnimation(DeltaSeconds);
    RightEarAlpha = RightEarAnimation->GetAlpha();
  }
}

void UFaceAnimInstance::PlayFaceAnimation(UAnimSequenceBase* Animation)
{
  if (ULog::ErrorIf(Animation == nullptr, TEXT("Animation instance is nullptr"))) return;

  PlaySlotAnimationAsDynamicMontage(Animation, DefaultSlotName, 0.2f);
}

void UFaceAnimInstance::StopFaceAnimation()
{
  StopSlotAnimation(0.5f, DefaultSlotName);
}
