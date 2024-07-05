// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AvatarId.h"
#include "Models/Models.h"
#include "RestApi.h"

#include "SimundiApi.generated.h"

UCLASS()
class SIMUNDIAI_API USimundiApi : public URestApi
{
  GENERATED_BODY()

public:
  TSharedRef<TRequestHandle<FGetAvatarsResponse>> GetAvatars();
  TSharedRef<TRequestHandle<FPostConversationResponse, FPostConversationPayload>> PostNewConversation(
      const FString Language, const FString AvatarId
  );
  TSharedRef<TRequestHandle<FPostUserMessageResponse, FUserMessage>> PostUserMessage(const FString Message, const FString ConversationId);
};
