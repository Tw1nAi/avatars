// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Actions.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FAction
{
  GENERATED_USTRUCT_BODY()

  inline static FName DefaultName = FName(TEXT("DefaultName"));

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FName Name = FName(TEXT("DefaultName"));

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Method;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FString> RequiredKeys;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Url;
};

USTRUCT(Blueprintable, BlueprintType)
struct FGetActionsResponse
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FAction> Actions;
};