// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UserMessage.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FUserMessage
{
  GENERATED_USTRUCT_BODY()

  FUserMessage()
  {
  }

  FUserMessage(FString Text)
      : Text(Text)
  {
  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Text;
};

USTRUCT(Blueprintable, BlueprintType)
struct FAvatarMessage
{
  GENERATED_USTRUCT_BODY()

  FAvatarMessage()
  {
  }

  FAvatarMessage(FString Text)
      : Text(Text)
  {
  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Id;

  /* Possible values: bot, human. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Sender;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Text;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Timestamp;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString AudioPath;
};

USTRUCT(Blueprintable, BlueprintType)
struct FPostUserMessageResponse
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FAvatarMessage Message;
};