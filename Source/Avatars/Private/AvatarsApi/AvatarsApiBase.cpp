// Copyright Juice sp. z o. o., All Rights Reserved.

#include "AvatarsApi/AvatarsApiBase.h"

#include "AvatarsTypes.h"

FString UAvatarsApiBase::VersionEnumToString(const EApiVersion ApiVersion)
{
  switch (ApiVersion)
  {
  case EApiVersion::API_v1:
    return "API_v1";
  case EApiVersion::API_v2:
    return "API_v2";
  default:
    return "None";
  }
}