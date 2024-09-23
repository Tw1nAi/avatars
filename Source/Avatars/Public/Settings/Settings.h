// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Settings.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FConversationSettings
{
  GENERATED_BODY()

  static inline const FString RestApiName = FString("Rest API");
  static inline const FString OpenAiApiName = FString("Open.ai API");

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Settings")
  FString TranscriptionApi = FString(TEXT("Rest Api"));
  FString GetTranscriptionApi() const;
  void SetTranscriptionApi(const FString& NewApi);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Settings")
  FString OpenApiKey = FString(TEXT(""));
  FString GetOpenApiKey() const;
  void SetOpenApiKey(const FString& NewApiKey);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Settings")
  float AvatarChangeTimeout = 0.0f;
  FText GetAvatarChangeTimeoutText() const;
  void SetAvatarChangeTimeout(const float NewTimeout);

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conversation Settings")
  float IdleGreetingTimeout = 0.0f;
  FText GetIdleGreetingTimeoutText() const;
  void SetIdleGreetingTimeout(const float NewTimeout);

  void CopyTo(FConversationSettings& Other) const
  {
    Other.TranscriptionApi = TranscriptionApi;
    Other.OpenApiKey = OpenApiKey;
    Other.AvatarChangeTimeout = AvatarChangeTimeout;
    Other.IdleGreetingTimeout = IdleGreetingTimeout;
  }
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