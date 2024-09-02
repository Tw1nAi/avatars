// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphicsSettingsWidgetBase.h"

#include "Components/CheckBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Framerate.h"

DEFINE_LOG_CATEGORY(LogGraphicsSettingsWidgetBase);

// make sure this doesn't leak outside this file scope
namespace
{
// clang-format off
  // static FString WindowModeLabel(TEXT("Window Mode"));
  static const FString ResolutionName(TEXT("Resolution"));
  static const FString VSyncName(TEXT("VSync"));
  static const FString FrameRateName(TEXT("Frame Rate"));
  static const FString ShadingName(TEXT("Shading Quality"));
  static const FString GlobalIlluminationName(TEXT("Global Illumination Quality"));
  static const FString PostProcessName(TEXT("PostProcess Quality"));
  static const FString VisualEffectsName(TEXT("Visual Effects Quality"));
  static const FString ShadowsName(TEXT("Shadows Quality"));

// clang-format on
} // namespace

UGraphicsSettingsWidgetBase::UGraphicsSettingsWidgetBase()
{
  LabelMinWidth = 200.0f;

  QualityOptionsLabels.Emplace(FText::FromString(TEXT("Low")));
  QualityOptionsLabels.Emplace(FText::FromString(TEXT("Medium")));
  QualityOptionsLabels.Emplace(FText::FromString(TEXT("High")));
  QualityOptionsLabels.Emplace(FText::FromString(TEXT("Epic")));
  QualityOptionsLabels.Emplace(FText::FromString(TEXT("Cinematic")));

  UncappedFrameRateName = FText::FromString(TEXT("Uncapped"));
  FrameRateOptions.Add(30);
  FrameRateOptions.Add(45);
  FrameRateOptions.Add(60);
  FrameRateOptions.Add(90);
  FrameRateOptions.Add(120);

  BuildNamedTypes();

  for (const FNamedSettingType& SettingName : DefaultNamesWithTypes)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Log, TEXT("Building default graphic settings for: %s"), *SettingName.Name);
    FSettingDefinition DefaultSetting(SettingName.Name, SettingName.Type);
    DefaultSettings.Emplace(DefaultSetting);
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
  AddNameWithType(ResolutionName, ESettingType::Custom);
  AddNameWithType(VSyncName, ESettingType::OnOff);
  AddNameWithType(FrameRateName, ESettingType::Custom);
  AddNameWithType(ShadingName, ESettingType::Quality, &UGameUserSettings::GetShadingQuality, &UGameUserSettings::SetShadingQuality);
  AddNameWithType(GlobalIlluminationName, ESettingType::Quality, &UGameUserSettings::GetGlobalIlluminationQuality, &UGameUserSettings::SetGlobalIlluminationQuality);
  AddNameWithType(PostProcessName, ESettingType::Quality, &UGameUserSettings::GetPostProcessingQuality, &UGameUserSettings::SetPostProcessingQuality);
  AddNameWithType(VisualEffectsName, ESettingType::Quality, &UGameUserSettings::GetVisualEffectQuality, &UGameUserSettings::SetVisualEffectQuality);
  AddNameWithType(ShadowsName, ESettingType::Quality, &UGameUserSettings::GetShadowQuality, &UGameUserSettings::SetShadowQuality);
}

void UGraphicsSettingsWidgetBase::ResetDefaultSettings()

{
  const int32 Length = DefaultSettings.Num();
  for (int32 i = 0; i < Length; i++)
  {
    const FNamedSettingType& NameWithType = DefaultNamesWithTypes[i];
    FSettingDefinition& DefaultSetting = DefaultSettings[i];
    if (bResetLabels) DefaultSetting.DefaultSettingName = NameWithType.Name;
    if (bResetWidgetTypes) DefaultSetting.SettingType = NameWithType.Type;
  }
}

void UGraphicsSettingsWidgetBase::GenerateDefaultOptionsWidgets()
{
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

  // BuildNamedTypes();

  for (int32 i = 0; i < DefaultSettings.Num(); i++)
  {
    FSettingDefinition& Setting = DefaultSettings[i];
    Setting.Getter = DefaultNamesWithTypes[i].Getter;
    Setting.Setter = DefaultNamesWithTypes[i].Setter;

    UWidget* SettingWidget = NewObject<USelectionWidgetBase>(this, SelectionWidgetBaseClass);

    if (SettingWidget == nullptr)
    {
      FString Message = FString::Printf(TEXT("Failed to create widget for setting %s"), *Setting.DefaultSettingName);
      UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("%s"), *Message);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
      continue;
    }

    USelectionWidgetBase* SelectionWidget = Cast<USelectionWidgetBase>(SettingWidget);

    if (SelectionWidget == nullptr)
    {
      FString Message = FString::Printf(TEXT("Failed to create selection widget for setting %s"), *Setting.DefaultSettingName);
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

    UPanelSlot* PanelSlot = HorizontalBox->AddChild(SettingWidget);
    if (UHorizontalBoxSlot* RowSlot = Cast<UHorizontalBoxSlot>(PanelSlot))
    {
      RowSlot->SetHorizontalAlignment(HAlign_Fill);
      RowSlot->SetVerticalAlignment(VAlign_Center);
    }

    FSelectionOption OffOption(FText::FromString(TEXT("Off")));
    FSelectionOption OnOption(FText::FromString(TEXT("On")));

    switch (Setting.SettingType)
    {
    case ESettingType::OnOff:

      SelectionWidget->AddOption(OffOption);
      SelectionWidget->AddOption(OnOption);
      break;

    case ESettingType::Quality:
      for (const FSelectionOption& Option : QualityOptionsLabels)
      {
        SelectionWidget->AddOption(Option.GetCopy());
      }
      break;
    }

    SelectionWidget->SetCurrentSelection(0);

    if (OptionsContainer && OptionsContainer->IsA<UPanelWidget>())
    {
      OptionsContainer->AddChild(HorizontalBox);
    }
    else
    {
      UE_LOG(LogGraphicsSettingsWidgetBase, Warning, TEXT("OptionsContainer is not a UPanelWidget"));
    }

    Setting.WidgetInstance = SelectionWidget;
  }
}

void UGraphicsSettingsWidgetBase::InitAllSettings()
{
  InitResolution();
  InitVSync();
  InitFrameRate();

  for (const FSettingDefinition& Setting : DefaultSettings)
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

void UGraphicsSettingsWidgetBase::InitResolution()
{
  USelectionWidgetBase* ResolutionsWidget = GetSettingWidget(ResolutionName);
  if (!ResolutionsWidget)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("ResolutionsWidget is not set"));
    return;
  }

  Resolutions.Reset();
  UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);

  ResolutionsWidget->Clear();
  for (const FIntPoint& Resolution : Resolutions)
  {
    FSelectionOption Option;
    Option.Label = FText::FromString(FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y));
    ResolutionsWidget->AddOption(Option);
  }

  // find current resolutions index
  FIntPoint CurrentResolution = GameUserSettings->GetScreenResolution();
  int32 CurrentResolutionIndex = Resolutions.IndexOfByPredicate([CurrentResolution](const FIntPoint& Resolution) { return Resolution == CurrentResolution; });

  check(CurrentResolutionIndex != INDEX_NONE && CurrentResolutionIndex > 0);

  ResolutionsWidget->SetCurrentSelection(CurrentResolutionIndex);

  ResolutionsWidget->OnSelectionChanged.BindUObject(this, &UGraphicsSettingsWidgetBase::OnResolutionChanged);
}

void UGraphicsSettingsWidgetBase::OnResolutionChanged(const int32 NewIndex)
{
  const FIntPoint Resolution = Resolutions[NewIndex];
  GameUserSettings->SetScreenResolution(Resolution);
  GameUserSettings->ApplySettings(false);
}

void UGraphicsSettingsWidgetBase::InitVSync()
{
  USelectionWidgetBase* VSyncWidget = GetSettingWidget(VSyncName);
  if (!VSyncWidget)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("VSyncWidget is not set"));
    return;
  }

  VSyncWidget->SetCurrentSelection(GameUserSettings->IsVSyncEnabled() ? 1 : 0);
  VSyncWidget->OnSelectionChanged.BindUObject(this, &UGraphicsSettingsWidgetBase::OnVSyncChanged);
}

void UGraphicsSettingsWidgetBase::OnVSyncChanged(const int32 NewIndex)
{
  GameUserSettings->SetVSyncEnabled(NewIndex == 0 ? false : true);
  GameUserSettings->ApplySettings(false);
}

void UGraphicsSettingsWidgetBase::AddFrameRateLimitOption(const int32 Limit, const FText& OptionalLabel)
{
  FSelectionOption Option;
  Option.Label = OptionalLabel.IsEmpty() ? FText::FromString(FString::Printf(TEXT("%d"), Limit)) : OptionalLabel;
  Option.IntValue = Limit;
  FrameRateWidget->AddOption(Option);
}

void UGraphicsSettingsWidgetBase::InitFrameRate()
{
  FrameRateWidget = GetSettingWidget(FrameRateName);
  if (!FrameRateWidget)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("FrameRateWidget is not set"));
    return;
  }

  FrameRateWidget->OptionName = FrameRateName;
  FrameRateWidget->Clear();

  // Add option value of 0 representing uncapped frame rate at index 0
  FrameRateOptions.Insert(0, 0);

  int OptIndex = 0;
  const int32 CurrentFrameRate = FMath::RoundToInt(GameUserSettings->GetFrameRateLimit());
  for (const int32 FrameLimit : FrameRateOptions)
  {
    if (OptIndex == 0)
    {
      AddFrameRateLimitOption(0, UncappedFrameRateName);
    }
    else
    {
      AddFrameRateLimitOption(FrameLimit);
    }

    if (CurrentFrameRate == FrameLimit)
    {
      FrameRateWidget->SetCurrentSelection(OptIndex);
    }

    OptIndex++;
  }

  FrameRateWidget->OnSelectionChanged.BindLambda([this](const int32 Index) {
    const int32 FrameRate = FrameRateOptions[Index];
    GameUserSettings->SetFrameRateLimit(FrameRate);
    GameUserSettings->ApplySettings(false);
  });
}
