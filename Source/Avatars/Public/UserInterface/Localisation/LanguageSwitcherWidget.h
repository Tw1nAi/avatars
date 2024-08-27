// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "AvatarsTypes.h"
#include "Components/WrapBox.h"
#include "CoreMinimal.h"
#include "WidgetBase.h"

#include "LanguageSwitcherWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLanguageSwitchSignature, EAvatarLanguage, Language);

class ULanguageButtonWidget;

UCLASS()
class AVATARS_API ULanguageSwitcherWidget : public UWidgetBase
{
  GENERATED_BODY()

public:
  virtual void NativeConstruct() override;
  virtual void SynchronizeProperties() override;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
  TObjectPtr<UWrapBox> ButtonsBox;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Language Switcher")
  TArray<EAvatarLanguage> Languages;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Language Switcher")
  TSubclassOf<ULanguageButtonWidget> LanguageButtonClass;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Language Switcher")
  TArray<TObjectPtr<ULanguageButtonWidget>> LanguageButtons;

  FOnLanguageSwitchSignature OnLanguageSwitchEvent;

  UFUNCTION()
  void OnLanguageButtonPressed(EAvatarLanguage Language);

  bool HasLanguageButton(EAvatarLanguage Language) const;
};
