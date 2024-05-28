// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/CharacterThumbnailWidget.h"

#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"

void UCharacterThumbnailWidget::NativeConstruct()
{
  Super::NativeConstruct();
}

void UCharacterThumbnailWidget::UpdateImage(UObject* InImage)
{
  FSlateBrush Brush;
  Brush.SetResourceObject(InImage);
  Brush.ImageSize = FVector2D(ThumbnailSize, ThumbnailSize);

  CharacterImage->SetBrush(Brush);
  ImageSource = InImage;
}

void UCharacterThumbnailWidget::UpdateSize(float Size)
{
  SizeBox->SetWidthOverride(Size);
  SizeBox->SetHeightOverride(Size);
  ThumbnailSize = Size;
}

void UCharacterThumbnailWidget::UpdateSelectionNative(const bool bInIsSelected)
{
  bIsSelected = bInIsSelected;
  UpdateSelection(bInIsSelected);
}

void UCharacterThumbnailWidget::Update()
{
  UpdateSize(ThumbnailSize);
  UpdateImage(ImageSource);
}

void UCharacterThumbnailWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
  PlayAnimation(MouseEnterAnimation);
  Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UCharacterThumbnailWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
  PlayAnimation(MouseLeaveAnimation);
  Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UCharacterThumbnailWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
  // UpdateSelectionNative(true);

  if (OnClickEvent.IsBound())
  {
    OnClickEvent.Broadcast(AvatarId);
  }

  return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
