// Copyright Juice sp. z o. o., All Rights Reserved.

#include "AvatarsApi/Api_v1/AvatarsApi.h"

void UAvatarsApi::AddAction(FAction Action)
{
  Actions.Emplace(Action);
}

void UAvatarsApi::AddActions(TArray<FAction> InActions)
{
  for (const FAction& Action : InActions)
  {
    AddAction(Action);
  }
}

FAction UAvatarsApi::GetActionByName(FName MethodName)
{
  auto* Action = Actions.FindByPredicate([MethodName](const FAction& Act) { return Act.Name == MethodName; });

  if (Action == nullptr)
  {
    // TODO: handle error
    return FAction();
  }

  return *Action;
}

TSharedRef<TRequestHandle<FGetActionsResponse>> UAvatarsApi::GetActionsRequest()
{
  auto Handle = MakeHandle<FGetActionsResponse>();
  return CreateHttpRequest<FGetActionsResponse>(Handle, URestApi::Get, BaseUrl);
}

TSharedRef<TRequestHandle<FGetAvatarsResponse>> UAvatarsApi::GetAvatars()
{
  auto Handle = MakeHandle<FGetAvatarsResponse>();
  return CreateHttpRequest<FGetAvatarsResponse>(Handle, URestApi::Get, "/avatar/");
}

TSharedRef<TRequestHandle<FPostConversationResponse, FPostConversationPayload>> UAvatarsApi::PostNewConversation()
{
  auto Handle = MakeHandle<FPostConversationResponse, FPostConversationPayload>();
  return CreateHttpRequest<FPostConversationResponse, FPostConversationPayload>(Handle, URestApi::Post, "/conversation/");
}

TSharedRef<TRequestHandle<FPostConversationResponse, FPostConversationPayload>> UAvatarsApi::PostNewConversation(
    const FString Language, const uint8 AvatarId
)
{
  auto Handle = MakeHandle<FPostConversationResponse, FPostConversationPayload>();
  FPostConversationPayload Payload;
  Payload.Lang = Language;
  Payload.AvatarId = AvatarId;
  return CreateHttpRequest<FPostConversationResponse, FPostConversationPayload>(Handle, URestApi::Post, "/conversation/", &Payload);
}

TSharedRef<TRequestHandle<FPostUserMessageResponse, FUserMessage>> UAvatarsApi::PostUserMessage(
    const FString Message, const int32 ConversationId
)
{
  auto Handle = MakeHandle<FPostUserMessageResponse, FUserMessage>();
  FString Url = FString::Printf(TEXT("/conversation/%d/message/"), ConversationId);
  FUserMessage Payload(Message);
  return CreateHttpRequest<FPostUserMessageResponse, FUserMessage>(Handle, URestApi::Post, Url, &Payload);
}
