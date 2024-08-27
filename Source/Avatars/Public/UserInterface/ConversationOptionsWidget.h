// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Components/CheckBox.h"
#include "Components/EditableText.h"
#include "CoreMinimal.h"
#include "WidgetBase.h"
#include "Utils/Public/Debouncer.h"

#include "ConversationOptionsWidget.generated.h"

class AAvatarsPlayerController;

UCLASS()
class AVATARS_API UConversationOptionsWidget : public UWidgetBase
{
  GENERATED_BODY()

public:
  virtual void NativeConstruct() override;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UEditableText* AvatarChangeDelayInput;
  UFUNCTION() void OnAvatarsChangeDelayChange(const FText& InText);
  /* Avatar change timeout */
  FDebouncer ChangeAvatarDebounce{0.75f};

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UEditableText* IdleGreetingTimeoutInput;
  UFUNCTION() void OnIdleGreetingTimeoutChange(const FText& InText);
  /* Idle greeting timeout */
  FDebouncer IdleGreetingTimeout{0.75f};

private:
  AAvatarsPlayerController* PlayerController;
  bool GetController();
};
