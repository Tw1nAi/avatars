// Copyright Juice sp. z o. o., All Rights Reserved.

#include "Persistance/PersistanceController.h"

#include "Kismet/GameplayStatics.h"
#include "Log.h"
#include "Persistance/SettingsSaveGame.h"

void UPersistanceController::SaveAll()
{
  USettingsSaveGame* SettingsSaveGame = Cast<USettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(USettingsSaveGame::StaticClass()));
  if (ULog::ErrorIf(SettingsSaveGame == nullptr, TEXT("Failed to create settings save game in UPersistanceController::SaveAll."))) return;

  ConversationSettings.CopyTo(SettingsSaveGame->ConversationSettings);
  SettingsSaveGame->AvatarsSettings = AvatarsSettings;

  UGameplayStatics::SaveGameToSlot(SettingsSaveGame, SettingsSaveName, 0);
}

bool UPersistanceController::LoadAll()
{
  USettingsSaveGame* SettingsSaveGame = Cast<USettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(SettingsSaveName, 0));
  if (ULog::ErrorIf(SettingsSaveGame == nullptr, TEXT("Failed to load settings save game in UPersistanceController::LoadAll."))) return false;

  SettingsSaveGame->ConversationSettings.CopyTo(ConversationSettings);
  AvatarsSettings = SettingsSaveGame->AvatarsSettings;

  return true;
}

FAvatarSettings& UPersistanceController::GetAvatarSettigns(const FString& AvatarId)
{
  FAvatarSettings* AvatarSettings = AvatarsSettings.Find(AvatarId);
  if (AvatarSettings == nullptr)
  {
    AvatarSettings = &AvatarsSettings.Add(AvatarId, FAvatarSettings());
  }
  return *AvatarSettings;
}

void UPersistanceController::SetAvatarSettings(const FString& AvatarId, const FAvatarSettings& AvatarSettings)
{
  AvatarsSettings.Add(AvatarId, AvatarSettings);
}