// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "AvatarsTypes.h"
#include "CoreMinimal.h"

#include "Avatar.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FAvatar
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  uint8 Id;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FName Name;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EAvatarCharacter Tag;
};

USTRUCT(Blueprintable, BlueprintType)
struct FGetAvatarsResponse
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FAvatar> Avatars;
};