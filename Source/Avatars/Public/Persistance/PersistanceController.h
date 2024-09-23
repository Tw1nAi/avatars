// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/Settings.h"

#include "PersistanceController.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (GameSaves))
class AVATARS_API UPersistanceController : public UObject
{
  GENERATED_BODY()

public:
  /* The name of the file used to save application settings. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString SettingsSaveName = TEXT("AppSettings");

  UPROPERTY(VisibleAnywhere, Category = "Application Settings")
  FConversationSettings ConversationSettings;

  /* Settings per avatar, saved with the avatar GUID as map key. */
  UPROPERTY(VisibleAnywhere, Category = "Avatars Settings")
  TMap<FString, FAvatarSettings> AvatarsSettings;

  UFUNCTION(BlueprintCallable)
  FAvatarSettings& GetAvatarSettigns(const FString& AvatarId);

  UFUNCTION(BlueprintCallable)
  void SetAvatarSettings(const FString& AvatarId, const FAvatarSettings& AvatarSettings);

  UFUNCTION(BlueprintCallable, Category = "Saves Manager")
  void SaveAll();

  UFUNCTION(BlueprintCallable, Category = "Saves Manager")
  bool LoadAll();
};