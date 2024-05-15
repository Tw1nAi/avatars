// Copyright Juice sp. z o. o., All Rights Reserved.

#include "FaceAnimInstance.h"

#include "Curves/CurveFloat.h"
#include "Get.h"
#include "Log.h"

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

  DialogAnimation = Animation;
  bIsFaceAnimationValid = DialogAnimation != nullptr;

  // FTimerDelegate LockFaceAnimEntryDelegate;
  // LockFaceAnimEntryDelegate.BindLambda([&]() {
  //   this->bIsFaceAnimationValid = false;
  // });
  // FTimerHandle Handle;
  // GetWorld()->GetTimerManager().SetTimer(Handle, LockFaceAnimEntryDelegate, DialogAnimation->GetPlayLength() * 0.5, false);

  // FTimerDelegate TimerDelegate;
  // TimerDelegate.BindLambda([&]() {
  //   this->bIsFaceAnimationValid = false;
  // });
  // FTimerHandle Handle;
  // GetWorld()->GetTimerManager().SetTimer(Handle, TimerDelegate, DialogAnimation->GetPlayLength() * 0.5, false);
}

void UFaceAnimInstance::StopFaceAnimation()
{
  bIsFaceAnimationValid = false;
}
