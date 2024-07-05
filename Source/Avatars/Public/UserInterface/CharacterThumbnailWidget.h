// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

#include "AvatarsTypes.h"

#include "CharacterThumbnailWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnClickDelegate, AActor*);

class UTexture2D;

UCLASS()
class AVATARS_API UCharacterThumbnailWidget : public UUserWidget
{
  GENERATED_BODY()

public:
  bool bIsSelected;

  UPROPERTY()
  AActor* Avatar;

  /* Should be of one the following: UMaterial, UMaterialInstance or UTexture2D */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AvatarThumbnail)
  UObject* ImageSource;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AvatarThumbnail)
  float ThumbnailSize = 250.f;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  USizeBox* SizeBox;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UImage* CharacterImage;

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UImage* HoverOverlay;

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  UWidgetAnimation* MouseEnterAnimation;

  UPROPERTY(Transient, meta = (BindWidgetAnim))
  UWidgetAnimation* MouseLeaveAnimation;

  UFUNCTION(BlueprintCallable)
  void UpdateImage(UObject* InImage);

  UFUNCTION(BlueprintCallable)
  void UpdateSelectionNative(const bool bInIsSelected);

  UFUNCTION(BlueprintImplementableEvent)
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
