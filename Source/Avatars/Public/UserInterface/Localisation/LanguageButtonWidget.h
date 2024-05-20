// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AvatarsTypes.h"
#include "Components/TextBlock.h"
#include "UserInterface/ButtonBaseWidget.h"

#include "LanguageButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLanguageButtonPressedSignature, EAvatarLanguage, Language);

UCLASS()
class AVATARS_API ULanguageButtonWidget : public UButtonBaseWidget
{
  GENERATED_BODY()

public:
  virtual void NativeConstruct() override;
  virtual void SynchronizeProperties() override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Language Button")
  EAvatarLanguage Language;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
  TObjectPtr<UTextBlock> LanguageText;

  UFUNCTION(BlueprintCallable)
  EAvatarLanguage GetLanguage() const { return Language; }

  UFUNCTION(BlueprintCallable)
  void SetLanguage(EAvatarLanguage InLanguage);

  UFUNCTION(BlueprintCallable)
  bool IsLanguage(EAvatarLanguage InLanguage) const { return Language == InLanguage; }

  FOnLanguageButtonPressedSignature OnLanguageSwitchEvent;

  UFUNCTION()
  void OnButtonPressed();

  UFUNCTION(BlueprintImplementableEvent, Category = "Language Button")
  void OnSelectedChange(const bool IsSelected);

  UFUNCTION(BlueprintCallable)
  void SetSelected(const bool bInSelected);
};
