// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/LoadingScreenWidget.h"

void ULoadingScreenWidget::NativeConstruct()
{
  Super::NativeConstruct();

  if (HideAnimation)
  {
    OnAnimHideDelegate.BindDynamic(this, &ULoadingScreenWidget::OnAnimHideFinished);
    BindToAnimationFinished(HideAnimation, OnAnimHideDelegate);
  }
}

void ULoadingScreenWidget::Hide()
{
  PlayAnimation(HideAnimation);
}

void ULoadingScreenWidget::Show(const FString NewLoadingText, const bool bShowSpinner)
{
  LoadingText->SetText(FText::FromString(NewLoadingText));
  Spinner->SetVisibility(bShowSpinner ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
  SetVisibility(ESlateVisibility::Visible);
  PlayAnimation(ShowAnimation);
}

void ULoadingScreenWidget::OnAnimHideFinished()
{
  SetVisibility(ESlateVisibility::Collapsed);
}
