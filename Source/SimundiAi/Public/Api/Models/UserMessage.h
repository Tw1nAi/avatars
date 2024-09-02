// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Api/Models/Conversation.h"
#include "CoreMinimal.h"

#include "UserMessage.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FUserMessage
{
  GENERATED_USTRUCT_BODY()

  FUserMessage() {}

  FUserMessage(FString InText) : Text(InText) {}

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Text;
};

USTRUCT(Blueprintable, BlueprintType)
struct FPostUserMessageResponse
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FMessage Message;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FString> Propositions;
};