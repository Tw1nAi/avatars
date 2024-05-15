// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "AvatarsTypes.h"
#include "CoreMinimal.h"

#include "Avatar_v2.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FAvatar_v2
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString AvatarId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EAvatarCharacter Tag;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FName AvatarName;
};

USTRUCT(Blueprintable, BlueprintType)
struct FGetAvatarsResponse_v2
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FAvatar_v2> Avatars;
};