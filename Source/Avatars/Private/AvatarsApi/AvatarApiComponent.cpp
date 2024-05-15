// Copyright Juice sp. z o. o., All Rights Reserved.

#include "AvatarsApi/AvatarApiComponent.h"

#include "Get.h"
#include "RestApi/RestApi.h"

UAvatarApiComponent::UAvatarApiComponent()
{
  PrimaryComponentTick.bCanEverTick = true;

  // ...
}

void UAvatarApiComponent::CleanUrl(FString& InOutUrl) const
{
  if (InOutUrl.StartsWith("/"))
  {
    InOutUrl.LeftChopInline(1);
  }

  if (InOutUrl.EndsWith("/"))
  {
    InOutUrl.RightChopInline(1);
  }
}

FString UAvatarApiComponent::GetFullUrl() const
{
  return FString::Printf(TEXT("%s:%d/%s"), *BaseUrl, Port, *UrlPath);
}

void UAvatarApiComponent::BeginPlay()
{
  Super::BeginPlay();
}

void UAvatarApiComponent::GetAvatars()
{
  ULog::Warn("This method should be overridden in a derived class.");
}

bool UAvatarApiComponent::CheckAndFormatMessage(FString& InOutMessage) const
{
  if (InOutMessage == "")
  {
    if (bDebug) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("The user submitted an EMPTY message"));
    return false;
  }

  if (bDebug) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("The user submitted message: " + InOutMessage));

  if (InOutMessage.Contains("[FINAL]"))
  {
    InOutMessage.ReplaceInline(TEXT("[FINAL]"), TEXT(""), ESearchCase::IgnoreCase);
  }

  InOutMessage.TrimStartAndEndInline();

  return true;
}

void UAvatarApiComponent::StartNewConversation(const EAvatarLanguage Language, const uint8 AvatarId)
{
  ULog::Warn("This method should be overridden in a derived class.");
}

void UAvatarApiComponent::SendUserMessage(FString Message)
{
  ULog::Warn("This method should be overridden in a derived class.");
}
