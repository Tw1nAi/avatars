// Fill out your copyright notice in the Description page of Project Settings.

#include "SelectionWidgetBase.h"

DEFINE_LOG_CATEGORY(LogButtonSelectionWidget);

USelectionWidgetBase::USelectionWidgetBase()
{
  SelectedOptionIndex = 0;
  SetInteractive(true);
  SetIsFocusable(true);
  Show();
}

void USelectionWidgetBase::NativeConstruct()
{
  Super::NativeConstruct();

  if (Options.Num() == 0)
  {
    UE_LOG(LogButtonSelectionWidget, Warning, TEXT("No options provided during NativeConstruct for the selection widget: %s"), *OptionName);
  }
  else
  {
    UpdateSelectionText();
  }

  FCustomWidgetNavigationDelegate NavigationDelegate;
  NavigationDelegate.BindDynamic(this, &USelectionWidgetBase::OnNavigation);
  SetNavigationRuleCustom(EUINavigation::Left, NavigationDelegate);
  SetNavigationRuleCustom(EUINavigation::Right, NavigationDelegate);
}

void USelectionWidgetBase::Clear()
{
  Options.Reset();
}

void USelectionWidgetBase::AddOption(const FSelectionOption& Option)
{
  Options.Add(Option);
}

FSelectionOption& USelectionWidgetBase::GetSelectedOption()
{
  return Options[SelectedOptionIndex];
}

void USelectionWidgetBase::SetCurrentSelection(const int32 Index)
{
  if (Index < 0 || Index >= Options.Num())
  {
    FString Message = FString::Printf(TEXT("Invalid index %d provided for the selection widget: %s"), Index, *OptionName);
    UE_LOG(LogButtonSelectionWidget, Error, TEXT("%s"), *Message);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
    return;
  }

  SelectedOptionIndex = Index;
  UpdateSelectionText();
}

UWidget* USelectionWidgetBase::OnNavigation(EUINavigation InNavigation)
{
  verify(InNavigation == EUINavigation::Left || InNavigation == EUINavigation::Right);

  const int32 Direction = InNavigation == EUINavigation::Left ? -1 : 1;

  const int32 NumberOfOptions = Options.Num();
  if (NumberOfOptions == 0)
  {
    FString Message = FString::Printf(TEXT("No options provided for the selection widget: %s"), *OptionName);
    UE_LOG(LogButtonSelectionWidget, Error, TEXT("%s"), *Message);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
    return this;
  }

  SelectedOptionIndex += Direction;
  if (SelectedOptionIndex < 0)
  {
    SelectedOptionIndex = NumberOfOptions - 1;
  }
  else if (SelectedOptionIndex >= NumberOfOptions)
  {
    SelectedOptionIndex = 0;
  }

  SelectedOptionIndex = FMath::Clamp(SelectedOptionIndex, 0, NumberOfOptions - 1);

  UpdateSelectionText();

  OnSelectionChanged.ExecuteIfBound(SelectedOptionIndex);

  return this;
}

void USelectionWidgetBase::UpdateSelectionText()
{
  // FString Message =
  //     FString::Printf(TEXT("Updating selection widget: %s, new option: %s, index: %d"), *OptionName, *Options[SelectedOptionIndex].Label.ToString(), SelectedOptionIndex);
  // UE_LOG(LogButtonSelectionWidget, Log, TEXT("%s"), *Message);
  // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Message);

  if (!LabelWidget)
  {
    FString Message = FString::Printf(TEXT("No label widget provided for the selection widget: %s"), *OptionName);
    UE_LOG(LogButtonSelectionWidget, Error, TEXT("%s"), *Message);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
    return;
  }

  if (!Options.IsValidIndex(SelectedOptionIndex))
  {
    FString Message = FString::Printf(TEXT("Invalid index %d provided for the selection widget: %s"), SelectedOptionIndex, *OptionName);
    UE_LOG(LogButtonSelectionWidget, Error, TEXT("%s"), *Message);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
    return;
  }

  LabelWidget->SetText(Options[SelectedOptionIndex].Label);
}

void USelectionWidgetBase::SelectPrevious()
{
  OnNavigation(EUINavigation::Left);
}

void USelectionWidgetBase::SelectNext()
{
  OnNavigation(EUINavigation::Right);
}
