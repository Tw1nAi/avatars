// Juice Sp. z o. o.. All rights reserved.

#pragma once

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
// #include "Engine/World.h"
// #include "Kismet/GameplayStatics.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AvatarsTypes.h"
#include "Log.h"

#include "Get.generated.h"

USTRUCT(BlueprintType)
struct AVATARS_API FGet
{
  GENERATED_USTRUCT_BODY()

  FGet() {};

  template <typename AssetClass>
  static AssetClass* GetAssetByPathAsync(const FString Location, const FString AssetName, TFunction<void(UObject* Asset)> OnAssetLoaded)
  {
    if (ULog::ErrorIf(Location.IsEmpty(), "Location is empty")) return nullptr;
    if (ULog::ErrorIf(AssetName.IsEmpty(), "AssetName is empty")) return nullptr;

    const FString Path = Location + AssetName + "." + AssetName;
    const FSoftObjectPath ReferencePath(Path);

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

  /* Find all assets of selected type in give directory. Be carefull when searching directories with lots of files, as this is blocking
   * operation. */
  template <typename AssetClass>
  static TArray<AssetClass*> GetAllAssetsByPath(
      const FString& Location, const bool bRecursivePaths = false, const bool bRecursiveClasses = true
  )
  {
    TArray<AssetClass*> Assets;

    if (ULog::ErrorIf(Location.IsEmpty(), "Location is empty")) return Assets;

    auto AssetRegistryModule = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FAssetData> AssetDataArray;

    // Ensure the path format is correct for the AssetRegistry
    const FString SearchPath = Location.EndsWith(TEXT("/")) ? Location : Location + TEXT("/");

    FARFilter Filter;
    Filter.PackagePaths.Add(*SearchPath);
    Filter.bRecursivePaths = bRecursivePaths;
    Filter.bRecursiveClasses = bRecursiveClasses;
    Filter.ClassPaths.Add(AssetClass::StaticClass()->GetClassPathName()); // Use ClassPaths instead of ClassNames

    AssetRegistryModule->Get().GetAssets(Filter, AssetDataArray);
    // AssetDataArray.StableSort([](const FAssetData& A, const FAssetData& B) { return A.AssetName.ToString() < B.AssetName.ToString(); });

    for (const FAssetData& AssetData : AssetDataArray)
    {
      UObject* AssetObject = AssetData.GetAsset();

      if (AssetObject)
      {
        AssetClass* Asset = Cast<AssetClass>(AssetObject);
        if (Asset)
        {
          Assets.Add(Asset);
        }
        else
        {
          ULog::Error("Could not cast asset object to target class at Path: " + AssetData.GetObjectPathString());
        }
      }
      else
      {
        ULog::Error("Could not find asset at Path: " + AssetData.GetObjectPathString());
      }
    }

    return Assets;
  }

  static FString LanguageEnumAsIsoString(EAvatarLanguage Language)
  {
    switch (Language)
    {
    case EAvatarLanguage::EN:
      return "EN";
    case EAvatarLanguage::PL:
      return "PL";
    default:
      checkNoEntry();
      return "";
    }
  }
};