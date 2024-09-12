// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "CoreMinimal.h"

#include "SelectionWidgetBase.h"
#include "SettingsTypes.h"
#include "WidgetBase.h"

#include "GraphicsSettingsWidgetBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGraphicsSettingsWidgetBase, Log, All);

class UGameUserSettings;
class UComboBoxStringWidgetBase;

UENUM(BlueprintType, Blueprintable)
enum class ESettingType : uint8
{
  OnOff UMETA(DisplayName = "ON / OFF"),
  Quality UMETA(DisplayName = "Quality"),
  WindowMode UMETA(DIsplayName = "Window Mode"),
  Custom UMETA(DisplayName = "Custom")
};

USTRUCT()
struct FNamedSettingType
{
  GENERATED_BODY()

  FNamedSettingType() = default;

  // clang-format off
  FNamedSettingType(
    const FString InName,
    const ESettingType InType,
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
  ESettingType Type;
  GetFunc Getter = nullptr;
  SetFunc Setter = nullptr;
};

USTRUCT(BlueprintType)
struct FSettingDefinition
{
  GENERATED_BODY()

  FSettingDefinition() = default;
  FSettingDefinition(const FString& InDefaultSettingName, ESettingType const InComponentType, const GetFunc InGetter = nullptr, const SetFunc InSetter = nullptr)
      : DefaultSettingName(InDefaultSettingName), SettingType(InComponentType), Getter(InGetter), Setter(InSetter)
  {
  }

  /* Localised label for the setting display. This should be saved between regenerations of the settings widgets. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FText Label = FText::GetEmpty();

  /* Name of the default setting provided with the engine. Will be filled automatically for default settings and should be empty for custom settings. */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FString DefaultSettingName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ESettingType SettingType;

  UPROPERTY()
  USelectionWidgetBase* WidgetInstance = nullptr;
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

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Graphics Settings", meta = (BindWidget))
  UButton* ApplyButton;

protected:
  UFUNCTION()
  void OnApplyButtonClicked();

public:

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TSubclassOf<UPanelWidget> OptionsContainerClass = UVerticalBox::StaticClass();

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TSubclassOf<USelectionWidgetBase> SelectionWidgetBaseClass;

  /* Default settings are provided with the engine. Here you can change the text labels that can be localised. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TArray<FSettingDefinition> DefaultSettings;

protected:
  void InitWindowMode();
  void OnWindowModeChange(const int32 Index);

public:
  /* Array of labeled default options used in named lists. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TArray<FText> WindowModeLabels;

  /* Array of labeled default options used in named lists. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TArray<FText> QualityOptionsLabels;

  // Frame rate settings
protected:
  void InitFrameRate();
  UPROPERTY(BlueprintReadOnly)
  TObjectPtr<USelectionWidgetBase> FrameRateWidget;
  void AddFrameRateLimitOption(const int32 Limit, const FText& OptionalLabel = FText::GetEmpty());

public:
  /* List of available frame rate limits in the game settings. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  TArray<int32> FrameRateOptions;

  /* Label for uncapped frame rate, rest of the frame rate limits are displayed as numbers. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics Settings")
  FText UncappedFrameRateName;

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
  template <typename T> T* CreateSettingWidget()
  {
    UWidget* SettingWidget = NewObject<USelectionWidgetBase>(this, SelectionWidgetBaseClass);

    T* SettingWidgetCast = Cast<T>(SettingWidget);

    return SettingWidgetCast;
  }

  USelectionWidgetBase* GetSettingWidget(const FString& SettingName)
  {
    for (FSettingDefinition& Setting : DefaultSettings)
    {
      if (Setting.DefaultSettingName == SettingName)
      {
        return Setting.WidgetInstance;
      }
    }

    return nullptr;
  }

  UPROPERTY()
  TObjectPtr<UGameUserSettings> GameUserSettings;

  void InitResolution();
  UFUNCTION()
  void OnResolutionChanged(const int32 NewIndex);
  /* This should automatically be filled with resolutions available for main screen. */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  TArray<FIntPoint> Resolutions;

  void InitVSync();
  UFUNCTION()
  void OnVSyncChanged(const int32 NewIndex);

  void InitAllSettings();

  UPROPERTY()
  TArray<FNamedSettingType> DefaultNamesWithTypes;

  void AddNameWithType(const FString& Name, const ESettingType Type, const GetFunc Getter = nullptr, const SetFunc Setter = nullptr)
  {
    DefaultNamesWithTypes.Emplace(FNamedSettingType(Name, Type, Getter, Setter));
  }

  void CheckAndAddDefaultValues();
  void BuildNamedTypes();
};
