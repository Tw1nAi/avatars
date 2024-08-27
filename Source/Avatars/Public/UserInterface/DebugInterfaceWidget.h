// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "AvatarsTypes.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "WidgetBase.h"

#include "DebugInterfaceWidget.generated.h"

UCLASS()
class AVATARS_API UDebugInterfaceWidget : public UWidgetBase
{
  GENERATED_BODY()

public:
  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
  UTextBlock* AvatarState;

  void SetAvatarState(const FString NewState);
};
