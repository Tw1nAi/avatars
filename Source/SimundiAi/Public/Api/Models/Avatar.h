// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Avatar.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FAvatar
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString AvatarId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString AvatarName;
};

USTRUCT(Blueprintable, BlueprintType)
struct FGetAvatarsResponse
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FAvatar> Avatars;
};