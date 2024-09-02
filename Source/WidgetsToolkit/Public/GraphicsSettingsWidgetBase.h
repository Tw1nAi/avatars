// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/WidgetTree.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/VerticalBox.h"
#include "CoreMinimal.h"

#include "ComboBoxStringWidgetBase.h"
#include "Framerate.h"
#include "SelectionWidgetBase.h"
#include "WidgetBase.h"

#include "GraphicsSettingsWidgetBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGraphicsSettingsWidgetBase, Log, All);

class UGameUserSettings;
class UComboBoxStringWidgetBase;

typedef int32 (UGameUserSettings::*GetFunc)() const;
typedef void (UGameUserSettings::*SetFunc)(int32);

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

UENUM()
enum class ESettingComponentType : uint8
{
  ComboBoxString,
  CheckBox,
  Selection,
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

/**
 * Base class containing the common functionality for all graphics settings widgets without visual representation.
 */
UCLASS()
class WIDGETSTOOLKIT_API UGraphicsSettingsWidgetBase : public UWidgetBase
{
  GENERATED_BODY()

public:
  UGraphicsSettingsWidgetBase();
  virtual void NativeConstruct() override;
  virtual void NativePreConstruct() override;

  UFUNCTION(BlueprintCallable)
  void GenerateDefaultOptionsWidgets();

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Graphics Settings", meta = (BindWidget))
  UPanelWidget* OptionsContainer;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TSubclassOf<UPanelWidget> OptionsContainerClass = UVerticalBox::StaticClass();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TSubclassOf<UCheckBox> CheckBoxClass;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TSubclassOf<UComboBoxStringWidgetBase> ComboBoxStringClass;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TSubclassOf<USelectionWidgetBase> SelectionWidgetBaseClass;

  /* Default settings are provided with the engine. Here you can change the text labels that can be localised. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TArray<FSettingDef> DefaultSettings;

  /* Array of labeled default options used in named lists. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TArray<FSelectionOption> DefaultOptionsRange;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings|Label")
  FSlateFontInfo LabelFont;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings|Label")
  float LabelMinWidth;

  /** Margin around each setting item row, label included. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  FMargin SettingsItemMargin;

  /** Use this in case editor is stuck on some of the component types properties to avoid recreating widget from scratch. */
  UFUNCTION(CallInEditor, Category = "Graphics Settings|Reset")
  void ResetDefaultSettings();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings|Reset")
  bool bResetLabels = false;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings|Reset")
  bool bResetWidgetTypes = false;

protected:
  template <typename T> T* CreateSettingWidget(const ESettingComponentType ComponentType)
  {
    UWidget* SettingWidget = nullptr;
    TSubclassOf<UWidget> WidgetClass;

    switch (ComponentType)
    {
    case ESettingComponentType::ComboBoxString:
      SettingWidget = NewObject<UComboBoxStringWidgetBase>(this, ComboBoxStringClass);
      break;
    case ESettingComponentType::CheckBox:
      SettingWidget = NewObject<UCheckBox>(this, CheckBoxClass);
      break;
    case ESettingComponentType::Selection:
      SettingWidget = NewObject<USelectionWidgetBase>(this, SelectionWidgetBaseClass);
      break;
    default:
      UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("Unknown setting component type %d"), static_cast<int32>(ComponentType));
      return nullptr;
    }

    T* SettingWidgetCast = Cast<T>(SettingWidget);

    return SettingWidgetCast;
  }

  template <typename T> T* GetSettingWidget(const FString& SettingName)
  {
    for (FSettingDef& Setting : DefaultSettings)
    {
      if (Setting.DefaultSettingName == SettingName)
      {
        return Cast<T>(Setting.WidgetInstance);
      }
    }

    return nullptr;
  }

  UPROPERTY()
  TObjectPtr<UGameUserSettings> GameUserSettings;

  void InitResolutionComboBox();
  UFUNCTION()
  void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
  UPROPERTY(BlueprintReadOnly)
  TObjectPtr<UComboBoxString> ResolutionComboBox;

  /* This should automatically be filled with resolutions available for main screen. */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<FIntPoint> Resolutions;

  void InitVSync();
  UFUNCTION()
  void OnVSyncChanged(const bool bIsChecked);
  UPROPERTY(BlueprintReadOnly)
  TObjectPtr<UCheckBox> VSyncCheckBox;

  void InitFramerate();
  UPROPERTY(BlueprintReadOnly)
  TObjectPtr<USelectionWidgetBase> FramerateWidget;

  // UserSettings->SetFullscreenMode(EWindowMode::Windowed);

  void InitAllSettings();

  UPROPERTY()
  TArray<FNameWithType> DefaultNamesWithTypes;

  void AddNameWithType(FString Name, ESettingComponentType Type, GetFunc Getter = nullptr, SetFunc Setter = nullptr)
  {
    DefaultNamesWithTypes.Push(FNameWithType(Name, Type, Getter, Setter));
  }

  void BuildNamedTypes();
};
