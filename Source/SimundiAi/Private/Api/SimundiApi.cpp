// Copyright Juice sp. z o. o., All Rights Reserved.

#include "Api/SimundiApi.h"

TSharedRef<TRequestHandle<FGetAvatarsResponse>> USimundiApi::GetAvatars()
{
  auto Handle = MakeHandle<FGetAvatarsResponse>();
  return CreateHttpRequest<FGetAvatarsResponse>(Handle, URestApi::Get, "/avatars/");
}

TSharedRef<TRequestHandle<FPostConversationResponse, FPostConversationPayload>> USimundiApi::PostNewConversation(
    const FString Language, const FString AvatarId
)
{
  auto Handle = MakeHandle<FPostConversationResponse, FPostConversationPayload>();
  FPostConversationPayload Payload(Language, AvatarId);
  return CreateHttpRequest<FPostConversationResponse, FPostConversationPayload>(Handle, URestApi::Post, "/conversations/", &Payload);
}

TSharedRef<TRequestHandle<FPostUserMessageResponse, FUserMessage>> USimundiApi::PostUserMessage(
    const FString Message, const FString ConversationId
)
{
  auto Handle = MakeHandle<FPostUserMessageResponse, FUserMessage>();
  FString Url = "/conversations/" + ConversationId + "/messages/";
  FUserMessage Payload(Message);
  return CreateHttpRequest<FPostUserMessageResponse, FUserMessage>(Handle, URestApi::Post, Url, &Payload);
}
