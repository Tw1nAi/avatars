// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/TextButtonWidget.h"

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