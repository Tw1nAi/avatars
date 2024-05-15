// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/DebugInterfaceWidget.h"

void UDebugInterfaceWidget::SetAvatarState(const FString NewState)
{
  if (AvatarState != nullptr)
  {
    AvatarState->SetText(FText::FromString(NewState));
  }
}