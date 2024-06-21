// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "AiIntellectInterface.h"

#include "AiIntellectComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AIINTELLECT_API UAiIntellectComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  UAiIntellectComponent();

protected:
  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
  TSubclassOf<UObject> IntellectClass;

  UPROPERTY()
  TScriptInterface<IAiIntellectInterface> Intellect;
};
