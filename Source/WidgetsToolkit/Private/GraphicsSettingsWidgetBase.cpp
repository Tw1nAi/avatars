// Fill out your copyright notice in the Description page of Project Settings.

#include "GraphicsSettingsWidgetBase.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"

// #include "Framerate.h"

DEFINE_LOG_CATEGORY(LogGraphicsSettingsWidgetBase);

// make sure this doesn't leak outside this file scope
namespace
{
// clang-format off
  static const FString WindowModeName(TEXT("Window Mode"));
  static const FString ResolutionName(TEXT("Resolution"));
  static const FString VSyncName(TEXT("VSync"));
  static const FString OverallQualityName(TEXT("Overall Quality"));
  static const FString FrameRateName(TEXT("Frame Rate"));
  static const FString ShadingName(TEXT("Shading Quality"));
  static const FString GlobalIlluminationName(TEXT("Global Illumination Quality"));
  static const FString VisualEffectsName(TEXT("Visual Effects Quality"));
  static const FString ShadowsName(TEXT("Shadows Quality"));
  static const FString ViewDistanceName(TEXT("View Distance Quality"));
  static const FString ReflectionName(TEXT("Reflection Quality"));
  static const FString AntiAliasingName(TEXT("AntiAliasing Quality"));
  static const FString TexturesName(TEXT("Textures Quality"));
  static const FString FoliageName(TEXT("Foliage Quality"));
  static const FString PostProcessingName(TEXT("Post Processing Quality"));

static int32 WindowModeToInt(EWindowMode::Type Mode)
{
  switch(Mode)
  {
  case EWindowMode::Type::Fullscreen: return 0;
  case EWindowMode::Type::WindowedFullscreen: return 1;
  case EWindowMode::Type::Windowed : return 2;
    default:
      UE_LOG(LogGraphicsSettingsWidgetBase, Warning, TEXT("%s"), *"Invalid Window Mode");
  }
  checkNoEntry();
  return 0;
}

// clang-format on
} // namespace

UGraphicsSettingsWidgetBase::UGraphicsSettingsWidgetBase()
{
  CheckAndAddDefaultValues();
}

void UGraphicsSettingsWidgetBase::NativeConstruct()
{
  GameUserSettings = UGameUserSettings::GetGameUserSettings();
  CheckAndAddDefaultValues();
  GenerateDefaultOptionsWidgets();
  InitAllSettings();
  SetSettingsState(true);
}

void UGraphicsSettingsWidgetBase::NativePreConstruct()
{
// Call this to visualise widgets in the editor
#if WITH_EDITOR
  CheckAndAddDefaultValues();
  GenerateDefaultOptionsWidgets();
  SetSettingsState(true);
#endif
}

void UGraphicsSettingsWidgetBase::CheckAndAddDefaultValues()
{
  if (LabelMinWidth == 0.0f)
  {
    LabelMinWidth = 200.0f;
  }

  if (WindowModeLabels.IsEmpty())
  {
    WindowModeLabels.Add(FText::FromString(TEXT("Fullscreen")));
    WindowModeLabels.Add(FText::FromString(TEXT("Windowed Fullscreen")));
    WindowModeLabels.Add(FText::FromString(TEXT("Windowed")));
  }

  if (QualityOptionsLabels.IsEmpty())
  {
    QualityOptionsLabels.Add(FText::FromString(TEXT("Low")));
    QualityOptionsLabels.Add(FText::FromString(TEXT("Medium")));
    QualityOptionsLabels.Add(FText::FromString(TEXT("High")));
    QualityOptionsLabels.Add(FText::FromString(TEXT("Epic")));
    QualityOptionsLabels.Add(FText::FromString(TEXT("Cinematic")));
  }

  if (UncappedFrameRateName.IsEmpty())
  {
    UncappedFrameRateName = FText::FromString(TEXT("Uncapped"));
  }

  if (CustomQualityLabel.IsEmpty())
  {
    CustomQualityLabel = FText::FromString(TEXT("Custom"));
  }

  if (FrameRateOptions.IsEmpty())
  {
    FrameRateOptions.Add(30);
    FrameRateOptions.Add(45);
    FrameRateOptions.Add(60);
    FrameRateOptions.Add(90);
    FrameRateOptions.Add(120);
  }

  BuildNamedTypes();

  // see if we can add missing named settings
  for (int32 i = 0; i < DefaultNamesWithTypes.Num(); i++)
  {
    const FNamedSettingType& NameWithType = DefaultNamesWithTypes[i];
    // check if the named settings exists
    const int32 Index = DefaultSettings.IndexOfByPredicate([NameWithType](const FSettingDefinition& Setting) { return Setting.DefaultSettingName == NameWithType.Name; });

    // if not add it at the index
    if (Index == INDEX_NONE || Index < 0)
    {
      FSettingDefinition DefaultSetting(NameWithType.Name, NameWithType.Type);
      DefaultSettings.Insert(DefaultSetting, i);
    }
  }

  // Make sanity check and loop over newly added settings if all are avilable in DefaultNamesWithTypes
  for (int32 i = 0; i < DefaultSettings.Num(); i++)
  {
    const FSettingDefinition& Setting = DefaultSettings[i];
    const int32 Index = DefaultNamesWithTypes.IndexOfByPredicate([Setting](const FNamedSettingType& NameWithType) { return NameWithType.Name == Setting.DefaultSettingName; });

    if (Index == INDEX_NONE || Index < 0)
    {
      UE_LOG(
          LogGraphicsSettingsWidgetBase,
          Warning,
          TEXT("Setting %s is not available in DefaultNamesWithTypes. Check if the setting is added to DefaultNamesWithTypes"),
          *Setting.DefaultSettingName
      );
      // remove this setting from this index
      DefaultSettings.RemoveAt(i);
    }
  }
}

void UGraphicsSettingsWidgetBase::BuildNamedTypes()
{
  DefaultNamesWithTypes.Reset();
  AddNameWithType(WindowModeName, ESettingType::WindowMode);
  AddNameWithType(ResolutionName, ESettingType::Custom);
  AddNameWithType(FrameRateName, ESettingType::Custom);
  AddNameWithType(VSyncName, ESettingType::OnOff);

  // quality settings
  AddNameWithType(OverallQualityName, ESettingType::Custom);

  AddNameWithType(ShadingName, ESettingType::Quality, &UGameUserSettings::GetShadingQuality, &UGameUserSettings::SetShadingQuality);
  AddNameWithType(GlobalIlluminationName, ESettingType::Quality, &UGameUserSettings::GetGlobalIlluminationQuality, &UGameUserSettings::SetGlobalIlluminationQuality);
  AddNameWithType(VisualEffectsName, ESettingType::Quality, &UGameUserSettings::GetVisualEffectQuality, &UGameUserSettings::SetVisualEffectQuality);
  AddNameWithType(ShadowsName, ESettingType::Quality, &UGameUserSettings::GetShadowQuality, &UGameUserSettings::SetShadowQuality);
  AddNameWithType(ViewDistanceName, ESettingType::Quality, &UGameUserSettings::GetViewDistanceQuality, &UGameUserSettings::SetViewDistanceQuality);
  AddNameWithType(ReflectionName, ESettingType::Quality, &UGameUserSettings::GetReflectionQuality, &UGameUserSettings::SetReflectionQuality);
  AddNameWithType(AntiAliasingName, ESettingType::Quality, &UGameUserSettings::GetAntiAliasingQuality, &UGameUserSettings::SetAntiAliasingQuality);
  AddNameWithType(TexturesName, ESettingType::Quality, &UGameUserSettings::GetTextureQuality, &UGameUserSettings::SetTextureQuality);
  AddNameWithType(FoliageName, ESettingType::Quality, &UGameUserSettings::GetFoliageQuality, &UGameUserSettings::SetFoliageQuality);
  AddNameWithType(PostProcessingName, ESettingType::Quality, &UGameUserSettings::GetPostProcessingQuality, &UGameUserSettings::SetPostProcessingQuality);
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

void UGraphicsSettingsWidgetBase::InitGenericSettings()
{
  for (const FSettingDefinition& Setting : DefaultSettings)
  {
    if (Setting.SettingType != ESettingType::Quality) continue;

    if (USelectionWidgetBase* SelectionWidget = Cast<USelectionWidgetBase>(Setting.WidgetInstance))
    {
      SelectionWidget->OptionName = Setting.DefaultSettingName;
      SelectionWidget->OnSelectionChanged.BindLambda([&](int32 Index) {
        std::invoke(Setting.Setter, GameUserSettings, Index);
        SetSettingsState(false);
      });

      const int32 CurrentIndex = std::invoke(Setting.Getter, GameUserSettings);
      SelectionWidget->SetCurrentSelection(CurrentIndex);
    }
  }
}

void UGraphicsSettingsWidgetBase::GenerateDefaultOptionsWidgets()
{
  if (!SelectionWidgetBaseClass)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("SelectionWidgetBaseClass is not set"));
    return;
  }

  OptionsContainer->ClearChildren();

  // these should always have the same length
  if (DefaultSettings.Num() != DefaultNamesWithTypes.Num())
  {
    UE_LOG(
        LogGraphicsSettingsWidgetBase,
        Error,
        TEXT("DefaultSettings and DefaultNamesWithTypes don't have equal length. DefaultSettings length: %d, DefaultNamesWithTypes: %d"),
        DefaultSettings.Num(),
        DefaultNamesWithTypes.Num()
    );
    return;
  }

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
      // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
      continue;
    }

    USelectionWidgetBase* SelectionWidget = Cast<USelectionWidgetBase>(SettingWidget);

    if (SelectionWidget == nullptr)
    {
      FString Message = FString::Printf(TEXT("Failed to create selection widget for setting %s"), *Setting.DefaultSettingName);
      UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("%s"), *Message);
      // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
      continue;
    }

    SelectionWidget->SetOptionName(Setting.DefaultSettingName);
    Setting.WidgetInstance = SelectionWidget;

    UHorizontalBox* HorizontalBox = NewObject<UHorizontalBox>(this);
    UTextBlock* Label = NewObject<UTextBlock>(this);

    if (Setting.Label.IsEmpty())
    {
      Setting.Label = FText::FromString(Setting.DefaultSettingName);
    }

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

    case ESettingType::WindowMode:
      for (const FText& OptionText : WindowModeLabels)
      {
        SelectionWidget->AddOption(FSelectionOption(OptionText));
      }
    case ESettingType::Custom:
      break;
    default:
      UE_LOG(LogGraphicsSettingsWidgetBase, Warning, TEXT("Unsupported setting type"));
      checkNoEntry();
      break;
    }

    // SelectionWidget->SetCurrentSelection(0);

    if (OptionsContainer && OptionsContainer->IsA<UPanelWidget>())
    {
      OptionsContainer->AddChild(HorizontalBox);
    }
    else
    {
      UE_LOG(LogGraphicsSettingsWidgetBase, Warning, TEXT("OptionsContainer is not a UPanelWidget"));
    }
  }
}

void UGraphicsSettingsWidgetBase::OnApplyButtonClicked()
{
  GameUserSettings->ApplySettings(false);
  SetSettingsState(true);
}

void UGraphicsSettingsWidgetBase::SetSettingsState(const bool bSettingsApplied)
{
  if (ApplyButton) ApplyButton->SetIsEnabled(!bSettingsApplied);

  if (SettingsStateMessage)
  {
    SettingsStateMessage->SetIsEnabled(!bSettingsApplied);
    SettingsStateMessage->SetText(bSettingsApplied ? NoChangedSettingsMessage : ThereAreChangedSettingsMessage);
  }
}

void UGraphicsSettingsWidgetBase::InitAllSettings()
{
  InitWindowMode();
  InitResolution();
  InitVSync();
  InitFrameRate();
  InitOverallQuality();
  InitGenericSettings();

  ApplyButton->OnClicked.Clear();
  ApplyButton->OnClicked.AddDynamic(this, &UGraphicsSettingsWidgetBase::OnApplyButtonClicked);
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
  if (!UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions))
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("Failed to get supported resolutions"));
    checkNoEntry();
    return;
  }

  ResolutionsWidget->Clear();
  for (const FIntPoint& Resolution : Resolutions)
  {
    FSelectionOption Option;
    Option.Label = FText::FromString(FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y));
    ResolutionsWidget->AddOption(Option);
  }

  // find current resolutions index
  const FIntPoint CurrentResolution = GameUserSettings->GetScreenResolution();
  const int32 CurrentResolutionIndex = Resolutions.IndexOfByPredicate([CurrentResolution](const FIntPoint& Resolution) { return Resolution == CurrentResolution; });

  if (CurrentResolutionIndex != INDEX_NONE && CurrentResolutionIndex >= 0 && CurrentResolutionIndex < Resolutions.Num())
  {
    ResolutionsWidget->SetCurrentSelection(CurrentResolutionIndex);
  }
  else
  {
    UE_LOG(
        LogGraphicsSettingsWidgetBase, Warning, TEXT("Could not find current resolution in system provided resolutions. Current resolution: %s."), *CurrentResolution.ToString()
    );

    // set full screen and the current screen resolution
    const FIntPoint ScreenResolution = GameUserSettings->GetDesktopResolution();
    const int32 ScreenResolutionIndex = Resolutions.IndexOfByPredicate([ScreenResolution](const FIntPoint& Resolution) { return Resolution == ScreenResolution; });

    if (ScreenResolutionIndex != INDEX_NONE && ScreenResolutionIndex >= 0 && ScreenResolutionIndex < Resolutions.Num())
    {
      ResolutionsWidget->SetCurrentSelection(ScreenResolutionIndex);
      UGameUserSettings::GetGameUserSettings()->SetScreenResolution(Resolutions[ScreenResolutionIndex]);
      UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
    }
    else
    {
      const int32 LastIndex = Resolutions.Num() - 1;
      ResolutionsWidget->SetCurrentSelection(LastIndex);
      UGameUserSettings::GetGameUserSettings()->SetScreenResolution(Resolutions[LastIndex]);
      UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
    }
  }

  ResolutionsWidget->OnSelectionChanged.BindUObject(this, &UGraphicsSettingsWidgetBase::OnResolutionChanged);
}

void UGraphicsSettingsWidgetBase::OnResolutionChanged(const int32 NewIndex)
{
  const FIntPoint Resolution = Resolutions[NewIndex];
  GameUserSettings->SetScreenResolution(Resolution);
  SetSettingsState(false);
}

void UGraphicsSettingsWidgetBase::InitOverallQuality()
{
  USelectionWidgetBase* OverallQualityWidget = GetSettingWidget(OverallQualityName);
  if (!OverallQualityWidget)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("OverallQualityWidget is not set"));
    return;
  }

  OverallQualityWidget->OptionName = OverallQualityName;
  OverallQualityWidget->Clear();

  for (const FText& QualityOption : QualityOptionsLabels)
  {
    FSelectionOption Option;
    Option.Label = QualityOption;
    OverallQualityWidget->AddOption(Option);
  }
  // last index should be the custom quality option
  FSelectionOption Option;
  Option.Label = CustomQualityLabel;
  OverallQualityWidget->AddOption(Option);

  const int32 CurrentQuality = GameUserSettings->GetOverallScalabilityLevel();
  OverallQualityWidget->SetCurrentSelection(CurrentQuality);
  OnOverallQualityChanged(CurrentQuality);

  OverallQualityWidget->OnSelectionChanged.BindUObject(this, &UGraphicsSettingsWidgetBase::OnOverallQualityChanged);
}

void UGraphicsSettingsWidgetBase::OnOverallQualityChanged(const int32 NewIndex)
{
  const bool bIsCustomQuality = NewIndex == QualityOptionsLabels.Num() || NewIndex == -1;
  if (bIsCustomQuality)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("Received custom quality index of overall quality setting of value: %d"), NewIndex);
  }

  UE_LOG(LogGraphicsSettingsWidgetBase, Log, TEXT("Overall quality changed to %d index."), NewIndex);
  for (const FSettingDefinition& Setting : DefaultSettings)
  {
    if (Setting.SettingType != ESettingType::Quality || !Setting.WidgetInstance)
    {
      continue;
    }

    Setting.WidgetInstance->SetIsEnabled(bIsCustomQuality);

    if (!bIsCustomQuality)
    {
      Setting.WidgetInstance->SetCurrentSelection(NewIndex);
      if (Setting.Setter != nullptr)
      {
        std::invoke(Setting.Setter, GameUserSettings, NewIndex);
      }
    }
  }

  GameUserSettings->SetOverallScalabilityLevel(NewIndex);
  // GameUserSettings->SetOverallScalabilityLevel(bIsCustomQuality ? -1 : NewIndex);
  SetSettingsState(false);
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
  SetSettingsState(false);
}

void UGraphicsSettingsWidgetBase::OnWindowModeChange(const int32 Index)
{
  GameUserSettings->SetFullscreenMode(EWindowMode::ConvertIntToWindowMode(Index));
  SetSettingsState(false);
}

void UGraphicsSettingsWidgetBase::InitWindowMode()
{
  USelectionWidgetBase* WindowModeWidget = GetSettingWidget(WindowModeName);
  if (!WindowModeWidget)
  {
    UE_LOG(LogGraphicsSettingsWidgetBase, Error, TEXT("WindowModeWidget  is not set"));
    return;
  }

  WindowModeWidget->OptionName = WindowModeName;
  WindowModeWidget->Clear();

  EWindowMode::Type CurrentMode = GameUserSettings->GetLastConfirmedFullscreenMode();
  const int32 ModeInt = WindowModeToInt(CurrentMode);

  for (int32 i = 0; i < WindowModeLabels.Num(); i++)
  {
    const FText& ModeText = WindowModeLabels[i];
    FSelectionOption Option;
    Option.Label = ModeText;
    Option.IntValue = i;
    WindowModeWidget->AddOption(Option);

    if (i == ModeInt)
    {
      WindowModeWidget->SetCurrentSelection(i);
    }
  }

  WindowModeWidget->OnSelectionChanged.BindUObject(this, &UGraphicsSettingsWidgetBase::OnWindowModeChange);
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
    SetSettingsState(false);
  });
}
