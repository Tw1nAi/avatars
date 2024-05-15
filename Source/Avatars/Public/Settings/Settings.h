// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Settings.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FConversationSettings
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float AvatarChangeTimeout = 0.0f;
  FText GetAvatarChangeTimeoutText() const;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float IdleGreetingTimeout = 0.0f;
  FText GetIdleGreetingTimeoutText() const;

  void SetAvatarChangeTimeout(const float NewTimeout);
  void SetIdleGreetingTimeout(const float NewTimeout);
};

USTRUCT(BlueprintType, Blueprintable)
struct FCameraSettings
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector CameraOffset;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float CameraZoom = 0.0f;

  void SetCameraOffset(const FVector NewOffset);
};

USTRUCT(BlueprintType, Blueprintable)
struct FAvatarSettings
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FCameraSettings CameraSettings;
};