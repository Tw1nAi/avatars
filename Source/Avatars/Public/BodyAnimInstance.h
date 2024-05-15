// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "AvatarsTypes.h"
#include "CoreMinimal.h"

#include "BodyAnimInstance.generated.h"

UCLASS()
class AVATARS_API UBodyAnimInstance : public UAnimInstance
{
  GENERATED_BODY()

public:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  EAvatarState AvatarState;

  UFUNCTION(BlueprintCallable)
  void SetAvatarState(EAvatarState NewAvatarState);
};
