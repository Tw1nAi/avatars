// Juice Sp. z o. o.. All rights reserved.

#pragma once

#include "AvatarsTypes.generated.h"

UENUM(BlueprintType)
enum class EAvatarCharacter : uint8
{
  None UMETA(DisplayName = "None"),
  JanZumbach UMETA(DisplayName = "Jan Zumbach"),
  JanKowalewski UMETA(DisplayName = "Jan Kowalewski"),
  WojtekTheBear UMETA(DisplayName = "Wojtek The Bear")
};

UENUM(BlueprintType)
enum class EAvatarLanguage : uint8
{
  PL,
  EN
};

UENUM(BlueprintType)
enum class EAvatarState : uint8
{
  // Default state, no user input
  Idle,
  // Listening to user input, e.g. recording audio
  Listening,
  // Processing user input, e.g. waiting for the response from the AI services
  Thinking,
  // Speaking to the user, e.g. playing the audio and animation response
  Talking
};

UENUM(BlueprintType)
enum class EApiVersion : uint8
{
  None UMETA(DisplayName = "None"),
  API_v1 UMETA(DisplayName = "API v1"),
  API_v2 UMETA(DisplayName = "API v2")
};

USTRUCT(BlueprintType, Blueprintable)
struct FAvatarId
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  uint8 v1 = 0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString v2 = "";

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EAvatarCharacter AvatarTag = EAvatarCharacter::None;

  bool IsValid() { return v1 > 0 && !v2.IsEmpty() && AvatarTag != EAvatarCharacter::None; }

  // Overloading the equality operator
  bool operator==(const FAvatarId& Other) const { return v1 == Other.v1 && v2 == Other.v2 && AvatarTag == Other.AvatarTag; }

  // It's also a good practice to implement the inequality operator
  bool operator!=(const FAvatarId& Other) const { return !(*this == Other); }
};