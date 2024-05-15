// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Conversation_v2.generated.h"

// USTRUCT(Blueprintable, BlueprintType)
// struct FConversation_v2
// {
//   GENERATED_USTRUCT_BODY()

//   UPROPERTY(EditAnywhere, BlueprintReadWrite)
//   int32 Id;

//   UPROPERTY(EditAnywhere, BlueprintReadWrite)
//   FString Avatar;

//   UPROPERTY(EditAnywhere, BlueprintReadWrite)
//   uint8 AvatarId;

//   UPROPERTY(EditAnywhere, BlueprintReadWrite)
//   FString DateCreated;

//   UPROPERTY(EditAnywhere, BlueprintReadWrite)
//   FString DateUpdated;

//   /* Possible values: pol, eng. */
//   UPROPERTY(EditAnywhere, BlueprintReadWrite)
//   FString Lang;

//   UPROPERTY(EditAnywhere, BlueprintReadWrite)
//   TArray<FAvatarMessage> Messages;

//   /* Possible values: finished, in progress. */
//   UPROPERTY(EditAnywhere, BlueprintReadWrite)
//   FString Status;
// };

// USTRUCT(Blueprintable, BlueprintType)
// struct FGetConversationsResponse_v2
// {
//   GENERATED_USTRUCT_BODY()

//   UPROPERTY(EditAnywhere, BlueprintReadWrite)
//   TArray<FConversation> Conversations;
// };

USTRUCT(Blueprintable, BlueprintType)
struct FPostConversationPayload_v2
{
  GENERATED_USTRUCT_BODY()

  FPostConversationPayload_v2(
      FString LanguageISO = "pl",
      FString AvatarId = "",
      bool ReturnConversation = true,
      float Threshold = 0.87, // 0.93
      bool RepeatMessages = true,
      bool MatchToAnswer = false
  )
      : LanguageISO(LanguageISO), AvatarId(AvatarId), ReturnConversation(ReturnConversation), Threshold(Threshold),
        RepeatMessages(RepeatMessages), MatchToAnswer(MatchToAnswer)
  {
  }

  /* Possible values: pol, eng. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString LanguageISO;

  /* Possible values: pol, eng. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString AvatarId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool ReturnConversation;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float Threshold;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool RepeatMessages;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool MatchToAnswer;
};

USTRUCT(Blueprintable, BlueprintType)
struct FMessage_v2
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString AudioPath;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString ConversationId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString MessageId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Sender;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Text;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Timestamp;
};

USTRUCT(Blueprintable, BlueprintType)
struct FPostConversationResponse_v2
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString ConversationId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FMessage_v2 Message;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FString> Propositions;
};