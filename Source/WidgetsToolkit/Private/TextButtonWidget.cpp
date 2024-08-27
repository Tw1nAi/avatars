// Copyright Juice sp. z o. o., All Rights Reserved.

#include "TextButtonWidget.h"

void UTextButtonWidget::SynchronizeProperties()
{
  Super::SynchronizeProperties();

  if (TextBlock != nullptr)
  {
    TextBlock->SetText(Text);
    TextBlock->SetFont(Font);
    TextBlock->SetColorAndOpacity(TextColor);
  }
}

void UTextButtonWidget::SetText(const FText& NewText)
{
  Text = NewText;
  if (TextBlock != nullptr)
  {
    TextBlock->SetText(Text);
  }
}