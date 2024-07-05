#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "AiIdentity.h"

#include "AiIdentityInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UAiIdentityInterface : public UInterface
{
  GENERATED_BODY()
};

/**
 * Base interface for AI identity.
 */
class AIIDENTITY_API IAiIdentityInterface
{
  GENERATED_BODY()

public:
  UFUNCTION()
  virtual FAiIdentity& GetIdentity() = 0;
};
