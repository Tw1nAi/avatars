// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Conversation.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FPostConversationPayload
{
  GENERATED_USTRUCT_BODY()

  FPostConversationPayload(
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
struct FMessage
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
struct FPostConversationResponse
{
  GENERATED_USTRUCT_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString ConversationId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FMessage Message;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TArray<FString> Propositions;
};