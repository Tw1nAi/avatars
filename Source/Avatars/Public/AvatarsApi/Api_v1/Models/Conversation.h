// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Actions.h"
#include "CoreMinimal.h"
#include "UserMessage.h"

#include "Conversation.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FConversation
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 Id;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Avatar;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  uint8 AvatarId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString DateCreated;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString DateUpdated;

  /* Possible values: pol, eng. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Lang;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FAvatarMessage> Messages;

  /* Possible values: finished, in progress. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Status;
};

USTRUCT(Blueprintable, BlueprintType)
struct FGetConversationsResponse
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FConversation> Conversations;
};

USTRUCT(Blueprintable, BlueprintType)
struct FPostConversationPayload
{
  GENERATED_USTRUCT_BODY()

  FPostConversationPayload()
  {
  }

  FPostConversationPayload(FString Lang, int32 AvatarId)
      : Lang(Lang), AvatarId(AvatarId)
  {
  }

  /* Possible values: pol, eng. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Lang;

  /* Possible values: pol, eng. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  int32 AvatarId;
};

USTRUCT(Blueprintable, BlueprintType)
struct FPostConversationResponse
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FAction> Actions;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FConversation Conversation;
};