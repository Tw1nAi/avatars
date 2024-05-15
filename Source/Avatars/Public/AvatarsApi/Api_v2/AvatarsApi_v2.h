// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "AvatarsApi/Api_v2/Models/Avatar_v2.h"
#include "AvatarsApi/Api_v2/Models/Conversation_v2.h"
#include "AvatarsApi/Api_v2/Models/UserMessage_v2.h"
#include "AvatarsApi/AvatarsApiBase.h"
#include "CoreMinimal.h"

#include "AvatarsApi_v2.generated.h"

UCLASS()
class AVATARS_API UAvatarsApi_v2 : public UAvatarsApiBase
{
  GENERATED_BODY()

public:
  TSharedRef<TRequestHandle<FGetAvatarsResponse_v2>> GetAvatars();
  TSharedRef<TRequestHandle<FPostConversationResponse_v2, FPostConversationPayload_v2>> PostNewConversation(
      const FString Language, const FString AvatarId
  );
  TSharedRef<TRequestHandle<FPostUserMessageResponse_v2, FUserMessage_v2>> PostUserMessage(
      const FString Message, const FString ConversationId
  );
};
