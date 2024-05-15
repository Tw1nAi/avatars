// Juice Sp. z o. o.. All rights reserved.

#pragma once

#include "AssetRegistry/AssetRegistryModule.h"
#include "AvatarsPlayerController.h"
#include "Log.h"

// Engine includes
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Get.generated.h"

USTRUCT(BlueprintType)
struct AVATARS_API FGet
{
  GENERATED_USTRUCT_BODY()

  FGet(){};

  template <typename AssetClass>
  static AssetClass* GetAssetByPathAsync(const FString Location, const FString AssetName, TFunction<void(AssetClass* Asset)> OnSuccess)
  {
    if (ULog::ErrorIf(Location.IsEmpty(), "Location is empty")) return nullptr;
    if (ULog::ErrorIf(AssetName.IsEmpty(), "AssetName is empty")) return nullptr;

    const FString Path = Location + AssetName + "." + AssetName;
    const FSoftObjectPath ReferencePath(Path);

    auto OnAssetLoaded = [&](UObject* Asset) {
      if (!Asset)
      {
        ULog::Error("Could not find asset: " + AssetName + "Location: " + Location);
        return;
      }

      AssetClass* TargetAsset = Cast<AssetClass>(Asset);
      if (TargetAsset)
      {
        OnSuccess(TargetAsset);
      }
      ULog::Error("Could not cast to target asst class: " + AssetName);
    };

    UAssetManager::GetStreamableManager().RequestAsyncLoad(ReferencePath, OnAssetLoaded);
  }

  template <typename AssetClass> static AssetClass* GetAssetByPath(const FString Location, const FString AssetName)
  {
    if (ULog::ErrorIf(Location.IsEmpty(), "Location is empty")) return nullptr;
    if (ULog::ErrorIf(AssetName.IsEmpty(), "AssetName is empty")) return nullptr;

    const FString Path = Location + AssetName + "." + AssetName;
    const FSoftObjectPath ObjectPath(Path);

    auto AssetRegistryModule = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    FAssetData AssetData = AssetRegistryModule->Get().GetAssetByObjectPath(ObjectPath);
    UObject* AssetObject = AssetData.GetAsset();

    if (AssetObject == nullptr)
    {
      ULog::Error("Could not find asset: " + AssetName + "Location: " + Location);
      return nullptr;
    }

    // Use the AssetRegistryModule to get the asset data
    AssetClass* AnimationAsset = Cast<AssetClass>(AssetObject);
    if (AnimationAsset)
    {
      return AnimationAsset;
    }

    ULog::Error("Could not cast asset object to target class, Location: " + Location);
    return nullptr;
  }
};