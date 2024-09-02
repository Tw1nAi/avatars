// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphicsSettingsWidgetBase.h"

#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogGraphicsSettingsWidgetBase);

// make sure this doesn't leak outside this file scope
namespace
{
// clang-format off
  static const EFramerate FramerateOptions[] = {
      EFramerate::FPS_30,
      EFramerate::FPS_45,
      EFramerate::FPS_60,
      EFramerate::FPS_90,
      EFramerate::FPS_120,
      EFramerate::FPS_Uncapped
    };

  static const FString RangeLabels[] = {
      TEXT("Low"),
      TEXT("Medium"),
      TEXT("High"),
      TEXT("Epic"),
      TEXT("Cinematic")
    };

  // static FString WindowModeLabel(TEXT("Window Mode"));
  static const FString ResolutionName(TEXT("Resolution"));
  static const FString VSyncName(TEXT("VSync"));
  static const FString FramerateName(TEXT("Frame Rate"));
  static const FString ShadingName(TEXT("Shading Quality"));
  static const FString GlobalIlluminationName(TEXT("Global Illumination Quality"));
  static const FString PostProccessName(TEXT("PostProcess Quality"));
  static const FString VisualEffectsName(TEXT("Visual Effects Quality"));
  static const FString ShadowsName(TEXT("Shadows Quality"));

// clang-format on
} // namespace

UGraphicsSettingsWidgetBase::UGraphicsSettingsWidgetBase()
{
  LabelMinWidth = 200.0f;

  for (const FString Label : RangeLabels)
  {
    DefaultOptionsRange.Push(FSelectionOption(FText::FromString(Label)));
  }

  BuildNamedTypes();

  for (const FNameWithType& SettingName : DefaultNamesWithTypes)
  {
    FSettingDef DefaultSetting(SettingName.Name, SettingName.Type);
    DefaultSettings.Push(DefaultSetting);
  }
}

void UGraphicsSettingsWidgetBase::NativeConstruct()
{
  GameUserSettings = UGameUserSettings::GetGameUserSettings();
  GenerateDefaultOptionsWidgets();
  InitAllSettings();
}

void UGraphicsSettingsWidgetBase::NativePreConstruct()
{
// Call this to visualise widgets in the editor
#if WITH_EDITOR
  GenerateDefaultOptionsWidgets();
#endif
}

void UGraphicsSettingsWidgetBase::BuildNamedTypes()
{
  DefaultNamesWithTypes.Reset();
  AddNameWithType(ResolutionName, ESettingComponentType::ComboBoxString, nullptr, nullptr);
  AddNameWithType(VSyncName, ESettingComponentType::CheckBox, nullptr, nullptr);
  AddNameWithType(FramerateName, ESettingComponentType::Selection, nullptr, nullptr);
  AddNameWithType(ShadingName, ESettingComponentType::Selection, &UGameUserSettings::GetShadingQuality, &UGameUserSettings::SetShadingQuality);
  AddNameWithType(GlobalIlluminationName, ESettingComponentType::Selection, &UGameUserSettings::GetGlobalIlluminationQuality, &UGameUserSettings::SetGlobalIlluminationQuality);
  AddNameWithType(PostProccessName, ESettingComponentType::Selection, &UGameUserSettings::GetPostProcessingQuality, &UGameUserSettings::SetPostProcessingQuality);
  AddNameWithType(VisualEffectsName, ESettingComponentType::Selection, &UGameUserSettings::GetVisualEffectQuality, &UGameUserSettings::SetVisualEffectQuality);
  AddNameWithType(ShadowsName, ESettingComponentType::Selection, &UGameUserSettings::GetShadowQuality, &UGameUserSettings::SetShadowQuality);
}

void UGraphicsSettingsWidgetBase::ResetDefaultSettings()

{
  const int32 Length = DefaultSettings.Num();
  for (int32 i = 0; i < Length; i++)
  {
    const FNameWithType& FNameWithType = DefaultNamesWithTypes[i];
    FSettingDef& DefaultSetting = DefaultSettings[i];
    if (bResetLabels) DefaultSetting.DefaultSettingName = FNameWithType.Name;
    if (bResetWidgetTypes) DefaultSetting.ComponentType = FNameWithType.Type;
  }
}

void UGraphicsSettingsWidgetBase::GenerateDefaultOptionsWidgets()
{
  if (!CheckBoxClass)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("CheckBoxClass is not set"));
    return;
  }
  if (!ComboBoxStringClass)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("ComboBoxStringClass is not set"));
    return;
  }
  if (!SelectionWidgetBaseClass)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("SelectionWidgetBaseClass is not set"));
    return;
  }

  if (!OptionsContainer)
  {
    OptionsContainer = NewObject<UPanelWidget>(this, OptionsContainerClass);
  }
  OptionsContainer->ClearChildren();

  BuildNamedTypes();

  for (int32 i = 0; i < DefaultSettings.Num(); i++)
  {
    FSettingDef& Setting = DefaultSettings[i];
    Setting.Getter = DefaultNamesWithTypes[i].Getter;
    Setting.Setter = DefaultNamesWithTypes[i].Setter;

    UWidget* SettingWidget = CreateSettingWidget<UWidget>(Setting.ComponentType);

    if (SettingWidget == nullptr)
    {
      FString Message = FString::Printf(TEXT("Failed to create widget for setting %s"), *Setting.DefaultSettingName);
      UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("%s"), *Message);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
      continue;
    }

    UHorizontalBox* HorizontalBox = NewObject<UHorizontalBox>(this);
    UTextBlock* Label = NewObject<UTextBlock>(this);

    Label->SetText(Setting.Label);
    Label->SetFont(LabelFont);
    Label->SetMinDesiredWidth(LabelMinWidth);
    UHorizontalBoxSlot* LabelSlot = Cast<UHorizontalBoxSlot>(HorizontalBox->AddChild(Label));
    LabelSlot->SetPadding(SettingsItemMargin);
    HorizontalBox->AddChild(SettingWidget);

    if (USelectionWidgetBase* SelectionWidget = Cast<USelectionWidgetBase>(SettingWidget))
    {
      if (Setting.Getter && Setting.Setter)
      {
        for (const FSelectionOption& Option : DefaultOptionsRange)
        {
          SelectionWidget->AddOption(Option.GetCopy());
        }
        SelectionWidget->SetCurrentSelection(0);
      }
    }

    if (OptionsContainer && OptionsContainer->IsA<UPanelWidget>())
    {
      OptionsContainer->AddChild(HorizontalBox);
    }
    else
    {
      UE_LOG(LogGraphicsSettingsWidgetBase, Warning, TEXT("OptionsContainer is not a UPanelWidget"));
    }

    Setting.WidgetInstance = SettingWidget;
  }
}

void UGraphicsSettingsWidgetBase::InitAllSettings()
{
  InitResolutionComboBox();
  InitVSync();
  InitFramerate();

  for (const FSettingDef& Setting : DefaultSettings)
  {
    UWidget* SettingWidget = Setting.WidgetInstance;

    if (!SettingWidget)
    {
      FString Message = FString::Printf(TEXT("Widget instance not created for setting %s"), *Setting.DefaultSettingName);
      UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("%s"), *Message);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
      continue;
    }

    if (USelectionWidgetBase* SelectionWidget = Cast<USelectionWidgetBase>(SettingWidget))
    {
      if (Setting.Getter && Setting.Setter)
      {
        SelectionWidget->OptionName = Setting.DefaultSettingName;
        SelectionWidget->OnSelectionChanged.BindLambda([this, Setter = Setting.Setter](int32 Index) {
          std::invoke(Setter, GameUserSettings, Index);
          GameUserSettings->ApplySettings(false);
        });

        const int32 CurrentIndex = std::invoke(Setting.Getter, GameUserSettings);
        SelectionWidget->SetCurrentSelection(CurrentIndex);
      }
    }
  }
}

void UGraphicsSettingsWidgetBase::InitResolutionComboBox()
{
  ResolutionComboBox = GetSettingWidget<UComboBoxStringWidgetBase>(ResolutionName)->ComboBox;
  if (!ResolutionComboBox)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("ResolutionComboBox is not set"));
    return;
  }

  Resolutions.Reset();
  UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);

  ResolutionComboBox->ClearOptions();
  for (const FIntPoint& Resolution : Resolutions)
  {
    ResolutionComboBox->AddOption(FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y));
  }

  // find current resolutions index
  FIntPoint CurrentResolution = GameUserSettings->GetScreenResolution();
  int32 CurrentResolutionIndex = Resolutions.IndexOfByPredicate([CurrentResolution](const FIntPoint& Resolution) { return Resolution == CurrentResolution; });

  check(CurrentResolutionIndex != INDEX_NONE && CurrentResolutionIndex > 0);

  ResolutionComboBox->SetSelectedIndex(CurrentResolutionIndex);

  ResolutionComboBox->OnSelectionChanged.Clear();
  ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &UGraphicsSettingsWidgetBase::OnResolutionChanged);
}

void UGraphicsSettingsWidgetBase::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
  const FIntPoint Resolution = Resolutions[ResolutionComboBox->GetSelectedIndex()];
  GameUserSettings->SetScreenResolution(Resolution);
  GameUserSettings->ApplySettings(false);
}

void UGraphicsSettingsWidgetBase::InitVSync()
{
  VSyncCheckBox = GetSettingWidget<UCheckBox>(VSyncName);
  if (!VSyncCheckBox)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("VSyncCheckBox is not set"));
    return;
  }

  VSyncCheckBox->SetCheckedState(GameUserSettings->IsVSyncEnabled() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
  VSyncCheckBox->OnCheckStateChanged.Clear();
  VSyncCheckBox->OnCheckStateChanged.AddDynamic(this, &UGraphicsSettingsWidgetBase::OnVSyncChanged);
}

void UGraphicsSettingsWidgetBase::OnVSyncChanged(const bool bIsChecked)
{
  GameUserSettings->SetVSyncEnabled(bIsChecked);
  GameUserSettings->ApplySettings(false);
}

void UGraphicsSettingsWidgetBase::InitFramerate()
{
  FramerateWidget = GetSettingWidget<USelectionWidgetBase>(FramerateName);
  if (!FramerateWidget)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("Frame Rate Widget is not set"));
    return;
  }

  FramerateWidget->OptionName = FramerateName;
  FramerateWidget->Clear();

  int Index = 0;

  const float CurrentFramerate = GameUserSettings->GetFrameRateLimit();
  for (const EFramerate Framerate : FramerateOptions)
  {
    FSelectionOption Option;
    Option.Label = FText::FromString(UFramerateUtils::EnumToString(Framerate));
    Option.IntValue = UFramerateUtils::EnumToValue(Framerate);
    FramerateWidget->AddOption(Option);

    if (CurrentFramerate == Option.IntValue)
    {
      FramerateWidget->SetCurrentSelection(Index);
    }

    Index++;
  }

  FramerateWidget->OnSelectionChanged.BindLambda([this](int32 Index) {
    const int32 Framerate = FramerateWidget->GetSelectedOption().IntValue;
    GameUserSettings->SetFrameRateLimit(Framerate);
    GameUserSettings->ApplySettings(false);
  });
}
