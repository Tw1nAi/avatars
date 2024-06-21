// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "AiIntellectInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAiTextResponseSignature, FString, Response);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UAiIntellectInterface : public UInterface
{
  GENERATED_BODY()
};

/**
 * Base interface for AI intellect.
 */
class AIINTELLECT_API IAiIntellectInterface
{
  GENERATED_BODY()

public:
  // Use this method to implement sending an AI message fro users.
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
  bool RespondTo(const FString& Message);
};
