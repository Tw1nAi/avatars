// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

#include "AvatarsTypes.h"

#include "CharacterThumbnailWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnClickDelegate, const FAvatarId&);

class UTexture2D;

UCLASS()
class AVATARS_API UCharacterThumbnailWidget : public UUserWidget
{
  GENERATED_BODY()

public:
  FAvatarId AvatarId;
  bool bIsSelected;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AvatarThumbnail)
  EAvatarCharacter AvatarTag;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AvatarThumbnail)
  FString CharacterName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AvatarThumbnail)
  UTexture2D* ImageTexture;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AvatarThumbnail)
  float ThumbnailSize = 250.f;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  USizeBox* SizeBox;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UImage* CharacterImage;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UImage* HoverOverlay;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UBorder* BorderSelected;

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  UWidgetAnimation* MouseEnterAnimation;

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  UWidgetAnimation* MouseLeaveAnimation;

  UFUNCTION(BlueprintCallable)
  void UpdateImage(UTexture2D* InImageTexture);

  UFUNCTION(BlueprintCallable)
  void UpdateSelection(const bool bInIsSelected);

  UFUNCTION(BlueprintCallable)
  void UpdateSize(float Size);

  UFUNCTION(BlueprintCallable)
  void Update();

  FOnClickDelegate OnClickEvent;

protected:
  virtual void NativeConstruct() override;
  virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
  virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
  virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
