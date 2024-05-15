// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "AvatarsApi/AvatarApiComponent.h"
#include "CoreMinimal.h"

#include "AvatarApiComponent_v1.generated.h"

class UAvatarsApi;

UCLASS(ClassGroup = (Avatars), Abstract, BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class AVATARS_API UAvatarApiComponent_v1 : public UAvatarApiComponent
{
  GENERATED_BODY()

public:
  UAvatarApiComponent_v1();
  void SetConversationId(uint8 Id) { ConversationId = Id; };

protected:
  /* Current sonversation's id. When not having conversation this should be set to -1. */
  uint8 ConversationId = -1;

  FString MapLanguageToApiString(const EAvatarLanguage Language) const;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides))
  UAvatarsApi* AvatarApi;

  UFUNCTION(BlueprintCallable)
  bool HasValidConversationId() const;

  UFUNCTION(BlueprintCallable)
  virtual void GetAvatars() override;

public:
  virtual void StartNewConversation(const EAvatarLanguage Language, const uint8 AvatarId) override;
  virtual void SendUserMessage(FString Message) override;
  virtual void BeginPlay() override;
};
