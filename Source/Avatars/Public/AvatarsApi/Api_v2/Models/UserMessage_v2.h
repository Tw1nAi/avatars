// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "AvatarsApi/Api_v2/Models/Conversation_v2.h"
#include "CoreMinimal.h"

#include "UserMessage_v2.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FUserMessage_v2
{
  GENERATED_USTRUCT_BODY()

  FUserMessage_v2()
  {
  }

  FUserMessage_v2(FString Text)
      : Text(Text)
  {
  }

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Text;
};

USTRUCT(Blueprintable, BlueprintType)
struct FPostUserMessageResponse_v2
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FMessage_v2 Message;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FString> Propositions;
};