// Copyright Juice sp. z o. o., All Rights Reserved.

#include "Settings/Settings.h"

FText FConversationSettings::GetAvatarChangeTimeoutText() const
{
  return FText::FromString(FString::SanitizeFloat(AvatarChangeTimeout));
}

FText FConversationSettings::GetIdleGreetingTimeoutText() const
{
  return FText::FromString(FString::SanitizeFloat(IdleGreetingTimeout));
}

void FConversationSettings::SetAvatarChangeTimeout(const float NewTimeout)
{
  AvatarChangeTimeout = NewTimeout;
}

void FConversationSettings::SetIdleGreetingTimeout(const float NewTimeout)
{
  IdleGreetingTimeout = NewTimeout;
}

void FCameraSettings::SetCameraOffset(const FVector NewOffset)
{
  CameraOffset = NewOffset;
}
