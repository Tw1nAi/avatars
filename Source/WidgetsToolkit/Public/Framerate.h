// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Framerate.generated.h"

UENUM(BlueprintType)
enum EFramerate
{
  FPS_30 UMETA(DisplayName = "30 FPS"),
  FPS_45 UMETA(DisplayName = "45 FPS"),
  FPS_60 UMETA(DisplayName = "60 FPS"),
  FPS_90 UMETA(DisplayName = "90 FPS"),
  FPS_120 UMETA(DisplayName = "120 FPS"),
  FPS_Uncapped UMETA(DisplayName = "Uncapped")
};

class UFramerateUtils
{
public:
  static int32 EnumToValue(const EFramerate Framerate)
  {
    switch (Framerate)
    {
      // clang-format off
      case EFramerate::FPS_30: return 30;
      case EFramerate::FPS_45: return 45;
      case EFramerate::FPS_60: return 60;
      case EFramerate::FPS_90: return 90;
      case EFramerate::FPS_120: return 120;
      case EFramerate::FPS_Uncapped: return 0;
      default: return 0;
      // clang-format on
    }
  }

  static FString EnumToString(const EFramerate Framerate)
  {
    switch (Framerate)
    {
      // clang-format off
      case EFramerate::FPS_30: return "30 FPS";
      case EFramerate::FPS_45: return "45 FPS";
      case EFramerate::FPS_60: return "60 FPS";
      case EFramerate::FPS_90: return "90 FPS";
      case EFramerate::FPS_120: return "120 FPS";
      case EFramerate::FPS_Uncapped: return "Uncapped";
      default: return "Unknown";
      // clang-format on
    }
  }
};