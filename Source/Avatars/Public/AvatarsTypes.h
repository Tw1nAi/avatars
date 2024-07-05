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
