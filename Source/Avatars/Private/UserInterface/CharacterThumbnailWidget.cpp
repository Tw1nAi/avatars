// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/CharacterThumbnailWidget.h"

#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"

void UCharacterThumbnailWidget::NativeConstruct()
{
  Super::NativeConstruct();
}

void UCharacterThumbnailWidget::UpdateImage(UTexture2D* InImageTexture)
{
  FSlateBrush Brush;
  Brush.SetResourceObject(InImageTexture);
  Brush.ImageSize = FVector2D(ThumbnailSize, ThumbnailSize);

  CharacterImage->SetBrush(Brush);
  ImageTexture = InImageTexture;
}

void UCharacterThumbnailWidget::UpdateSize(float Size)
{
  SizeBox->SetWidthOverride(Size);
  SizeBox->SetHeightOverride(Size);
  ThumbnailSize = Size;
}

void UCharacterThumbnailWidget::UpdateSelection(const bool bInIsSelected)
{
  BorderSelected->SetVisibility(bInIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UCharacterThumbnailWidget::Update()
{
  UpdateSize(ThumbnailSize);
  UpdateImage(ImageTexture);
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
  // UpdateSelection(true);

  if (OnClickEvent.IsBound())
  {
    OnClickEvent.Broadcast(AvatarId);
  }

  return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
