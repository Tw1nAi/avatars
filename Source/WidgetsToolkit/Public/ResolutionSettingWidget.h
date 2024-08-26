// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RHI.h"
#include "WidgetBase.h"

#include "ResolutionSettingWidget.generated.h"

/**
 * Base for resolution setting widget that can retrive avilable resolutions.
 */
UCLASS()
class WIDGETSTOOLKIT_API UResolutionSettingWidget : public UWidgetBase
{
  GENERATED_BODY()

  // protected:
  //   UPROPERTY(EditAnywhere, BlueprintReadWrite)
  //   FScreenResolutionArray Resolutions;
  //
  // public:
  //   UPROPERTY(EditAnywhere, BlueprintReadWrite)
  //   bool bOnlyFullscreen = true;
  //
  //   bool GetAvailableResolutions();
};
