// Copyright Juice sp. z o. o., All Rights Reserved.

#include "AvatarsGame.h"

#include "AvatarsApi/Api_v1/AvatarsApi.h"
#include "AvatarsApi/Api_v2/AvatarsApi_v2.h"
#include "Persistance/PersistanceController.h"

UAvatarsGame::UAvatarsGame()
{
  CreateApiData.Add(FCreateApiData(UAvatarsApi::StaticClass(), EApiVersion::API_v1));
  CreateApiData.Add(FCreateApiData(UAvatarsApi_v2::StaticClass(), EApiVersion::API_v2));
}

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

UAvatarsApiBase* UAvatarsGame::GetApiByVersion(const EApiVersion ApiVersion)
{
  for (UAvatarsApiBase* Api : AvatarsApis)
  {
    if (Api->GetVersion() == ApiVersion)
    {
      return Api;
    }
  }
  const FString VersionString = UAvatarsApiBase::VersionEnumToString(ApiVersion);
  ULog::Info(TEXT("Could not find Api version: " + VersionString + ". Will try to create one."));

  for (const FCreateApiData& Data : CreateApiData)
  {
    if (Data.Version != ApiVersion) continue;

    UAvatarsApiBase* Api = NewObject<UAvatarsApiBase>(this, Data.ApiClass);
    if (ULog::ErrorIf(Api == nullptr, TEXT("Failed to create Api of version: " + VersionString))) return nullptr;

    Api->SetVersion(ApiVersion);
    AvatarsApis.Add(Api);
    ULog::Info(TEXT("Created Api of version: " + VersionString));
    return Api;
  }

  ULog::Error("Failed to get Api");
  return nullptr;
}

UAvatarsApiBase* UAvatarsGame::GetApi(const EApiVersion ApiVersion, UWorld* World)
{
  UAvatarsGame* GameInstance;
  if (!UAvatarsGame::Get(GameInstance, World)) return nullptr;

  UAvatarsApiBase* Api = GameInstance->GetApiByVersion(ApiVersion);
  if (!Api) return nullptr;

  return Api;
}