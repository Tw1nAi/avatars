#include "SimundiIntellectComponent.h"

#include "Api/Models/Models.h"
#include "Api/SimundiApi.h"

DEFINE_LOG_CATEGORY(LogSimundiIntellect);

USimundiIntellectComponent::USimundiIntellectComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void USimundiIntellectComponent::SetConversationId(const FString Id)
{
  ConversationId = Id;
}

bool USimundiIntellectComponent::HasStartedConversation() const
{
  return ConversationId != "";
}

void USimundiIntellectComponent::GetAvatarsData()
{
  /* Create initial requests. */
  Api->GetAvatars()
      ->Then([this](const FGetAvatarsResponse& Data) mutable {
        if (!IsValid(this))
        {
          FString Message = FString::Printf(TEXT("USimundiIntellectComponent's \"this\" reference is not valid in "
                                                 "USimundiIntellectComponent->GetAvatars()."));

          GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Message);
          UE_LOG(LogSimundiIntellect, Error, TEXT("%s"), *Message);
          return;
        }

        if (Data.Avatars.Num() == 0)
        {
          FString Message = FString::Printf(TEXT("No avatars data received from Api, USimundiIntellectComponent"));
          GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Message);
          UE_LOG(LogSimundiIntellect, Error, TEXT("%s"), *Message);
          return;
        }

        bool bFoundMatchingAvatar = false;
        for (const FAvatar& Avatar : Data.Avatars)
        {
          if (this->Identity.MatchByName(Avatar.AvatarName))
          {
            this->AvatarId = Avatar.AvatarId;
            bFoundMatchingAvatar = true;
            break;
          }
        }

        if (!bFoundMatchingAvatar)
        {
          FString Message =
              FString::Printf(TEXT("No matching avatar found for %s in USimundiIntellectComponent->GetAvatars()."), *this->Identity.Name);

          GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Message);
          UE_LOG(LogSimundiIntellect, Error, TEXT("%s"), *Message);
          return;
        }
      })
      ->Run();
}

void USimundiIntellectComponent::StartNewConversation(const FString& LanguageIso)
{
  if (AvatarId.IsEmpty())
  {
    FString Message = FString::Printf(TEXT("AvatarId is not set in USimundiIntellectComponent::StartNewConversation"));
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Message);
    UE_LOG(LogSimundiIntellect, Error, TEXT("%s"), *Message);
    return;
  }

  auto PostNewConversationHandle = Api->PostNewConversation(LanguageIso, AvatarId);

  PostNewConversationHandle
      ->Then([this, LanguageIso](const FPostConversationResponse& Data) mutable {
        if (!IsValid(this))
        {
          FString Message = FString::Printf(
              TEXT("USimundiIntellectComponent's this reference is not valid in USimundiIntellectComponent::StartNewConversation")
          );
          GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Message);
          UE_LOG(LogSimundiIntellect, Error, TEXT("%s"), *Message);
          return;
        }

        this->SetConversationId(Data.ConversationId);

        if (!this->PendingMessage.IsEmpty())
        {
          this->RespondTo(this->PendingMessage, LanguageIso);
          this->PendingMessage.Empty();
        }
      })
      ->Run();
}

bool USimundiIntellectComponent::RespondTo(const FString& Message, const FString& LanguageIso)
{
  if (AvatarId.IsEmpty())
  {
    FString ErrorMessage = FString::Printf(TEXT("Api is not set in USimundiIntellectComponent::RespondTo"));
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, ErrorMessage);
    UE_LOG(LogSimundiIntellect, Error, TEXT("%s"), *ErrorMessage);

    return false;
  }

  // The sole reason for saving a message as pending is to avoid sending it before the conversation have started.
  // There should be no other reason to save a message as pending.
  if (ConversationId == "")
  {
    PendingMessage = Message;
    StartNewConversation(LanguageIso);
    return true;
  }

  PendingMessage = "";

  Api->PostUserMessage(Message, ConversationId)
      ->Then([this](const FPostUserMessageResponse& Response) {
        if (!IsValid(this))
        {
          FString Message =
              FString::Printf(TEXT("USimundiIntellectComponent's this reference is not valid in USimundiIntellectComponent::RespondTo"));
          GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Message);
          UE_LOG(LogSimundiIntellect, Error, TEXT("%s"), *Message);
          return;
        }

        if (this->OnAiResponseEvent.IsBound())
        {
          this->OnAiResponseEvent.Broadcast(Response.Message.Text, Response.Message.AudioPath, Response.Propositions);
        }
      })
      ->Run();

  return true;
}

void USimundiIntellectComponent::BeginPlay()
{
  Super::BeginPlay();

  Api = NewObject<USimundiApi>(this);
  if (Api == nullptr)
  {
    FString Message = FString::Printf(TEXT("Could not create instance of USimundiApi."));
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Message);
    UE_LOG(LogSimundiIntellect, Error, TEXT("%s"), *Message);
    return;
  }

  Api->SetBaseUrl(Settings.BaseUrl);
  Api->AddDefaultHeader("Content-Type", "application/json");

  GetAvatarsData();
}
