// Copyright Juice sp. z o. o., All Rights Reserved.

#include "UserInterface/CameraOptionsWidget.h"

#include "AvatarPawn.h"
#include "AvatarsGame.h"
#include "AvatarsPlayerController.h"
#include "Get.h"
#include "Persistance/PersistanceController.h"

void UCameraOptionsWidget::NativeConstruct()
{
  if (CenterButton != nullptr)
  {
    CenterButton->Button->OnPressed.AddDynamic(this, &UCameraOptionsWidget::Center);
  }
  if (TopButton != nullptr)
  {
    TopButton->OnHoldEvent.AddDynamic(this, &UCameraOptionsWidget::MoveUp);
  }
  if (RightButton != nullptr)
  {
    RightButton->OnHoldEvent.AddDynamic(this, &UCameraOptionsWidget::MoveRight);
  }
  if (DownButton != nullptr)
  {
    DownButton->OnHoldEvent.AddDynamic(this, &UCameraOptionsWidget::MoveDown);
  }
  if (LeftButton != nullptr)
  {
    LeftButton->OnHoldEvent.AddDynamic(this, &UCameraOptionsWidget::MoveLeft);
  }

  if (ZoomSlider != nullptr)
  {
    ZoomSlider->OnValueChanged.AddDynamic(this, &UCameraOptionsWidget::OnZoomSliderValueChanged);
    ZoomSlider->SetValue(0.5f);
  }
}

void UCameraOptionsWidget::SynchronizeProperties()
{
  Super::SynchronizeProperties();
}

void UCameraOptionsWidget::SaveSettings()
{
  if (UPersistanceController* Persistance = UAvatarsGame::GetPersistance(GetWorld()))
  {
    AAvatarPawn* Avatar = AAvatarsPlayerController::Get(GetWorld())->GetSelectedAvatar();
    if (!Avatar) return;

    FAvatarSettings& Settings = Persistance->GetAvatarSettigns(Avatar->AvatarData.Name);
    Settings.CameraSettings.CameraOffset = Avatar->CameraOffset;
    Persistance->SetAvatarSettings(Avatar->AvatarData.Name, Settings);
    Persistance->SaveAll();
  }
}

void UCameraOptionsWidget::OnPositionButtonClicked(FVector2D Direction)
{
  AAvatarsPlayerController* PlayerController = AAvatarsPlayerController::Get(GetWorld());
  if (!PlayerController) return;

  PlayerController->OnCameraMoveInput(Direction);

  SaveSettings();
}

void UCameraOptionsWidget::Center()
{
  AAvatarsPlayerController* PlayerController = AAvatarsPlayerController::Get(GetWorld());
  if (!PlayerController) return;

  PlayerController->ResetCameraLocation();
  ResetZoom();

  SaveSettings();
}

void UCameraOptionsWidget::MoveUp()
{
  OnPositionButtonClicked(FVector2D(1.0f, 0.0f));
}

void UCameraOptionsWidget::MoveRight()
{
  OnPositionButtonClicked(FVector2D(0.0f, 1.0f));
}

void UCameraOptionsWidget::MoveDown()
{
  OnPositionButtonClicked(FVector2D(-1.0f, 0.0f));
}

void UCameraOptionsWidget::MoveLeft()
{
  OnPositionButtonClicked(FVector2D(0.0f, -1.0f));
}

void UCameraOptionsWidget::ResetZoom()
{
  if (ZoomSlider != nullptr)
  {
    SetZoom(0.5f);
    ZoomSlider->SetValue(0.5f);
  }
}

void UCameraOptionsWidget::SetZoom(const float Value)
{
  AAvatarsPlayerController* PlayerController = AAvatarsPlayerController::Get(GetWorld());
  if (!PlayerController) return;

  PlayerController->SetCameraZoomByAlpha(Value);
  SaveSettings();
}

void UCameraOptionsWidget::OnZoomSliderValueChanged(float Value)
{
  SetZoom(Value);
}
