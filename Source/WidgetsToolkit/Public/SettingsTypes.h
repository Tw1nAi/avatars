// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "SettingsTypes.generated.h"

typedef int32 (UGameUserSettings::*GetFunc)() const;
typedef void (UGameUserSettings::*SetFunc)(int32);

UENUM()
enum class ESettingComponentType : uint8
{
  ComboBoxString,
  CheckBox,
  Selection,
};

USTRUCT()
struct FNameWithType
{
  GENERATED_BODY()

  FNameWithType() = default;

  // clang-format off
  FNameWithType(
    const FString InName,
    const ESettingComponentType InType,
    const GetFunc InGetter = nullptr,
    const SetFunc InSetter = nullptr
  ) :
    Name(InName),
    Type(InType),
    Getter(InGetter),
    Setter(InSetter)
    {}
  // clang-format on

  FString Name;
  ESettingComponentType Type;
  GetFunc Getter = nullptr;
  SetFunc Setter = nullptr;
};

USTRUCT(BlueprintType)
struct FSettingDef
{
  GENERATED_BODY()

  FSettingDef() = default;
  FSettingDef(const FString& InDefaultSettingName, ESettingComponentType const InComponentType, const GetFunc InGetter = nullptr, const SetFunc InSetter = nullptr)
      : DefaultSettingName(InDefaultSettingName), ComponentType(InComponentType), Getter(InGetter), Setter(InSetter)
  {
  }

  /* Localised label for the setting display. This should be saved between regenerations of the settings widgets. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FText Label = FText::GetEmpty();

  /* Name of the default setting provided with the engine. Will be filled automatically for default settings. */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FString DefaultSettingName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ESettingComponentType ComponentType;

  UPROPERTY()
  UWidget* WidgetInstance = nullptr;
  GetFunc Getter = nullptr;
  SetFunc Setter = nullptr;
};