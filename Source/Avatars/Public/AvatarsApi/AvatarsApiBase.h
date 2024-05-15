// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "AvatarsTypes.h"
#include "CoreMinimal.h"
#include "RestApi/RestApi.h"

#include "AvatarsApiBase.generated.h"

UCLASS()
class AVATARS_API UAvatarsApiBase : public URestApi
{
  GENERATED_BODY()

protected:
  EApiVersion Version = EApiVersion::None;

public:
  UAvatarsApiBase(){};
  EApiVersion GetVersion() const { return Version; }
  void SetVersion(const EApiVersion InVersion) { Version = InVersion; }

  static FString VersionEnumToString(const EApiVersion ApiVersion);
};
