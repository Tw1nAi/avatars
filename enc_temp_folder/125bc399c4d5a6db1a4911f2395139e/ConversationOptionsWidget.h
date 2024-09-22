// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

// Engine
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableText.h"
#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"
#include "WidgetBase.h"

// Custom
#include "Utils/Public/Debouncer.h"

#include "ConversationOptionsWidget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogConversationOptionsWidget, Log, All);

class AAvatarsPlayerController;

UCLASS()
class AVATARS_API UConversationOptionsWidget : public UWidgetBase
{
  GENERATED_BODY()

public:
  virtual void NativeConstruct() override;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UComboBoxString* TranscriptionApiComboBox;
  UFUNCTION() void OnTranscriptionApiChange(FString ApiString, ESelectInfo::Type SelectionType);
  /* Transcription change timeout */
  void SetTranscriptionApi(const FString& Api);
  FString LastSavedApi;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UEditableText* ApiKey;

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
