// Copyright Juice sp. z o. o., All Rights Reserved.

#include "AvatarsApi/Api_v2/AvatarsApi_v2.h"

TSharedRef<TRequestHandle<FGetAvatarsResponse_v2>> UAvatarsApi_v2::GetAvatars()
{
  auto Handle = MakeHandle<FGetAvatarsResponse_v2>();
  return CreateHttpRequest<FGetAvatarsResponse_v2>(Handle, URestApi::Get, "/avatars/");
}

// TSharedRef<TRequestHandle<FPostConversationResponse_v2, FPostConversationPayload_v2>> UAvatarsApi_v2::PostNewConversation()
// {
//   auto Handle = MakeHandle<FPostConversationResponse_v2, FPostConversationPayload_v2>();
//   return CreateHttpRequest<FPostConversationResponse_v2, FPostConversationPayload_v2>(Handle, URestApi::Post, "/conversations/");
// }

TSharedRef<TRequestHandle<FPostConversationResponse_v2, FPostConversationPayload_v2>> UAvatarsApi_v2::PostNewConversation(const FString Language, const FString AvatarId)
{
  auto Handle = MakeHandle<FPostConversationResponse_v2, FPostConversationPayload_v2>();
  FPostConversationPayload_v2 Payload(Language, AvatarId);
  return CreateHttpRequest<FPostConversationResponse_v2, FPostConversationPayload_v2>(Handle, URestApi::Post, "/conversations/", &Payload);
}

TSharedRef<TRequestHandle<FPostUserMessageResponse_v2, FUserMessage_v2>> UAvatarsApi_v2::PostUserMessage(const FString Message, const FString ConversationId)
{
  auto Handle = MakeHandle<FPostUserMessageResponse_v2, FUserMessage_v2>();
  FString Url = "/conversations/" + ConversationId + "/messages/";
  FUserMessage_v2 Payload(Message);
  return CreateHttpRequest<FPostUserMessageResponse_v2, FUserMessage_v2>(Handle, URestApi::Post, Url, &Payload);
}
