// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Settings/Settings.h"

#include "SettingsSaveGame.generated.h"

UCLASS()
class AVATARS_API USettingsSaveGame : public USaveGame
{
  GENERATED_BODY()

public:
  UPROPERTY(VisibleAnywhere, Category = "Application Settings")
  FConversationSettings ConversationSettings;

  /* Settings per avatar, saved with the avatar GUID as map key. */
  UPROPERTY(VisibleAnywhere, Category = "Avatars Settings")
  TMap<FString, FAvatarSettings> AvatarsSettings;
};
