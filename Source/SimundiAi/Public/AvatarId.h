// Juice Sp. z o. o.. All rights reserved.

#pragma once

#include "AvatarId.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FAvatarId
{
  GENERATED_BODY()

  // legacy id, use string id instead
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  uint8 v1 = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString v2 = "";

  bool IsValid() { return v1 > 0 || !v2.IsEmpty(); }

  // Overloading the equality operator
  bool operator==(const FAvatarId& Other) const { return v1 == Other.v1 && v2 == Other.v2; }

  // It's also a good practice to implement the inequality operator
  bool operator!=(const FAvatarId& Other) const { return !(*this == Other); }
};