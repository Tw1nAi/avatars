// Copyright Juice sp. z o. o., All Rights Reserved.

#include "AvatarsApi/Api_v1/AvatarApiComponent_v1.h"

#include "AvatarsApi/Api_v1/AvatarsApi.h"
#include "AvatarsApi/Api_v1/Models/Actions.h"
#include "AvatarsApi/Api_v1/Models/Conversation.h"
#include "AvatarsApi/Api_v1/Models/UserMessage.h"
#include "Get.h"
#include "Log.h"

UAvatarApiComponent_v1::UAvatarApiComponent_v1() : Super()
{
  Port = 5001;
  BaseUrl = "http://localhost";
  UrlPath = "/api/v1/";
}

void UAvatarApiComponent_v1::BeginPlay()
{
  Super::BeginPlay();

  AvatarApi = NewObject<UAvatarsApi>(this, UAvatarsApi::StaticClass());
  if (ULog::ErrorIf(!AvatarApi, "Could not create AvatarApi."))
  {
    return;
  }

  AvatarApi->SetBaseUrl(GetFullUrl());
  AvatarApi->AddDefaultHeader("Content-Type", "application/json");
  AvatarApi->bDebug = bDebug;
}

bool UAvatarApiComponent_v1::HasValidConversationId() const
{
  return ConversationId > 0;
}

FString UAvatarApiComponent_v1::MapLanguageToApiString(const EAvatarLanguage Language) const
{
  switch (Language)
  {
  case EAvatarLanguage::PL:
    return TEXT("PL");
  case EAvatarLanguage::EN:
    return TEXT("EN");
  default:
    checkNoEntry();
    return TEXT("");
  }
}

void UAvatarApiComponent_v1::GetAvatars()
{
  AvatarApi->GetAvatars()
      ->Then([WeakComponent = TWeakObjectPtr<UAvatarApiComponent_v1>(this)](const FGetAvatarsResponse& Data) mutable {
        if (ULog::ErrorIf(!WeakComponent.IsValid(), "UAvatarApiComponent_v1's this reference is not valid."))
        {
          return;
        }

        // if (WeakComponent->OnGetAvatarsEvent.IsBound())
        // {
        //   WeakComponent->OnGetAvatarsEvent.Broadcast();
        // }
      })
      ->Run();
}

void UAvatarApiComponent_v1::StartNewConversation(const EAvatarLanguage Language, const uint8 AvatarId)
{
  auto PostNewConversationHandle = AvatarApi->PostNewConversation(MapLanguageToApiString(Language), AvatarId);

  PostNewConversationHandle
      ->Then([WeakComponent = TWeakObjectPtr<UAvatarApiComponent_v1>(this)](const FPostConversationResponse& Data) mutable {
        if (ULog::ErrorIf(
                !WeakComponent.IsValid(),
                "AvatarsPlayerController this reference is not valid in "
                "PostNewConversationHandle lambda in AAvatarsPlayerController"
            ))
        {
          return;
        }

        WeakComponent->SetConversationId(Data.Conversation.Id);
        if (Data.Conversation.Messages.Num() == 0)
        {
          // we should never have a conversation with no messages
          checkNoEntry();
          return;
        }

        // if (bDisplayAvatarResponse).
        // {
        //   WeakComponent->DisplayAvatarMessage(Data.Conversation.Messages[0]);
        //   return;
        // }
      })
      ->Run();
}

void UAvatarApiComponent_v1::SendUserMessage(FString Message)
{
  FString NewMessage = Message;

  if (!CheckAndFormatMessage(NewMessage))
  {
    return;
  }

  if (!AvatarApi || !HasValidConversationId())
  {
    if (bDebug)
    {
      ULog::ErrorIf(AvatarApi == nullptr, "Could not send message because AvatarApi is not set.");
      ULog::ErrorIf(!HasValidConversationId(), "Could not send message because AvatarApi is not ready.");
    }
    return;
  }

  AvatarApi->PostUserMessage(Message, ConversationId)
      ->Then([WeakComponent = TWeakObjectPtr<UAvatarApiComponent_v1>(this)](const FPostUserMessageResponse& Response) {
        if (ULog::ErrorIf(!WeakComponent.IsValid(), "UAvatarApiComponent_v1's this reference is not valid.")) return;

        // WeakComponent->DisplayAvatarMessage(Response.Message);
      })
      ->Run();
}
