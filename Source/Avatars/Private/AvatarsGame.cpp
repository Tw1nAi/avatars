// Copyright Juice sp. z o. o., All Rights Reserved.

#include "AvatarsGame.h"

#include "Persistance/PersistanceController.h"

UAvatarsGame::UAvatarsGame() {}

void UAvatarsGame::Init()
{
  Super::Init();
  SetupPersistance();
}

void UAvatarsGame::SetupPersistance()
{
  PersistanceController = NewObject<UPersistanceController>(this);
  if (ULog::ErrorIf(PersistanceController == nullptr, TEXT("Failed to create PersistanceController"))) return;

  if (!PersistanceController->LoadAll())
  {
    PersistanceController->ConversationSettings = DefaultConversationSettings;
  }
}

bool UAvatarsGame::Get(UAvatarsGame*& GameInstance, UWorld* World)
{
  GameInstance = Cast<UAvatarsGame>(World->GetGameInstance());
  if (ULog::ErrorIf(GameInstance == nullptr, TEXT("Failed to get game instance"))) return false;
  return true;
}

UPersistanceController* UAvatarsGame::GetPersistance(UWorld* World)
{
  UAvatarsGame* GameInstance;
  if (!UAvatarsGame::Get(GameInstance, World)) return nullptr;

  UPersistanceController* PersistanceController = GameInstance->PersistanceController;
  if (ULog::ErrorIf(PersistanceController == nullptr, TEXT("Failed to get PersistanceController"))) return nullptr;

  return PersistanceController;
}
