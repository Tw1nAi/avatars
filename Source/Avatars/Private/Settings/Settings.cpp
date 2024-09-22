// Copyright Juice sp. z o. o., All Rights Reserved.

#include "Settings/Settings.h"

FString FConversationSettings::GetTranscriptionApi() const
{
  return TranscriptionApi;
}

void FConversationSettings::SetTranscriptionApi(const FString& NewApi)
{
  TranscriptionApi = NewApi;
}

FText FConversationSettings::GetAvatarChangeTimeoutText() const
{
  return FText::FromString(FString::SanitizeFloat(AvatarChangeTimeout));
}

void FConversationSettings::SetAvatarChangeTimeout(const float NewTimeout)
{
  AvatarChangeTimeout = NewTimeout;
}

FText FConversationSettings::GetIdleGreetingTimeoutText() const
{
  return FText::FromString(FString::SanitizeFloat(IdleGreetingTimeout));
}

void FConversationSettings::SetIdleGreetingTimeout(const float NewTimeout)
{
  IdleGreetingTimeout = NewTimeout;
}

void FCameraSettings::SetCameraOffset(const FVector NewOffset)
{
  CameraOffset = NewOffset;
}
