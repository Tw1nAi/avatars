// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "AvatarsTypes.h"
#include "Log.h"
#include "Settings/Settings.h"

#include "AvatarsGame.generated.h"

class UPersistanceController;
class URootWidget;
class USettingsSaveGame;

UCLASS()
class AVATARS_API UAvatarsGame : public UGameInstance
{
  GENERATED_BODY()

public:
  UAvatarsGame();
  virtual void Init() override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars")
  bool bShowRuntimeProperties;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars")
  bool bDebug;

  static bool Get(UAvatarsGame*& GameInstance, UWorld* World);

  /* PERSISTANCE */
protected:
  UPROPERTY()
  UPersistanceController* PersistanceController;
  void SetupPersistance();

public:
  static UPersistanceController* GetPersistance(UWorld* World);

protected:
  UPROPERTY()
  URootWidget* GameplayUi;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars|Settings")
  FConversationSettings DefaultConversationSettings;

  // TODO: saved data loading failure handling
};
