// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "AvatarsApi/Api_v1/Models/Actions.h"
#include "AvatarsApi/Api_v1/Models/Avatar.h"
#include "AvatarsApi/Api_v1/Models/Conversation.h"
#include "AvatarsApi/Api_v1/Models/UserMessage.h"
#include "AvatarsApi/AvatarsApiBase.h"
#include "CoreMinimal.h"

#include "AvatarsApi.generated.h"

// ! change name UAvatarsApi to UAvatarsApi_v1
UCLASS()
class AVATARS_API UAvatarsApi : public UAvatarsApiBase
{
  GENERATED_BODY()

public:
  TSharedRef<TRequestHandle<FGetActionsResponse>> GetActionsRequest();
  TSharedRef<TRequestHandle<FGetAvatarsResponse>> GetAvatars();
  TSharedRef<TRequestHandle<FPostConversationResponse, FPostConversationPayload>> PostNewConversation();
  TSharedRef<TRequestHandle<FPostConversationResponse, FPostConversationPayload>> PostNewConversation(
      const FString Language, const uint8 AvatarId
  );
  TSharedRef<TRequestHandle<FPostUserMessageResponse, FUserMessage>> PostUserMessage(const FString Message, const int32 ConversationId);

  /* Basic REST actions retrived as first step from the server. The can be obtainted by calling GET on the base url. */
  UPROPERTY()
  TArray<FAction> Actions;

  UFUNCTION(BlueprintCallable)
  void AddAction(FAction Action);
  void AddActions(TArray<FAction> InActions);

  UFUNCTION()
  FAction GetActionByName(FName MethodName);
};
