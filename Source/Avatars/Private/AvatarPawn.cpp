// Copyright Juice sp. z o. o., All Rights Reserved.

#include "AvatarPawn.h"

#include "Animation/AnimSequence.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Sound/SoundWave.h"

#include "AudioStreamComponent.h"
#include "AvatarsApi/AvatarsApiBase.h"
#include "BodyAnimInstance.h"
#include "FaceAnimInstance.h"
#include "Get.h"
#include "Log.h"

// Sets default values
AAvatarPawn::AAvatarPawn()
{
  PrimaryActorTick.bCanEverTick = true;

  SceneComponent = CreateOptionalDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
  if (SceneComponent != nullptr)
  {
    SetRootComponent(SceneComponent);
  }

  AudioComponent = CreateOptionalDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
  if (AudioComponent != nullptr)
  {
    AudioComponent->OnAudioFinished.AddDynamic(this, &AAvatarPawn::OnAudioFinishedPlaying);
    AudioComponent->SetupAttachment(GetRootComponent());
  }

  // AudioStreamComponent = CreateOptionalDefaultSubobject<UAudioStreamComponent>(TEXT("AudioStreamComponent"));

  if (bApplySceneSetup)
  {
    ApplySceneSetup();
  }

  CameraMaxOffset = FVector(40.0, 100.0, 40.0);
}

void AAvatarPawn::BeginPlay()
{
  Super::BeginPlay();

  if (bApplySceneSetup)
  {
    ApplySceneSetup();
  }
}

void AAvatarPawn::StartAudioStream()
{
  if (AudioStreamComponent == nullptr)
  {
    ULog::Error("AudioStreamComponent is not set in AvatarPawn: " + GetName());
    return;
  }

  AudioStreamComponent->StartAudioStream();
}

void AAvatarPawn::StopAudioStream()
{
  if (AudioStreamComponent == nullptr)
  {
    ULog::Error("AudioStreamComponent is not set in AvatarPawn: " + GetName());
    return;
  }

  AudioStreamComponent->StopAudioStream();
}

bool AAvatarPawn::GetAvatarsController()
{
  AAvatarsPlayerController::Get(GetWorld());
  return AvatarsController != nullptr;
}

void AAvatarPawn::ApplySettings(FAvatarSettings& Settings)
{
  // if (Settings.bDebug)
  // {
  //   bDebug = Settings.bDebug;
  // }

  // if (Settings.MeshScale != 0.0)
  // {
  //   SceneSetup.MeshScale = Settings.MeshScale;
  //   if (BodyMeshComponent != nullptr)
  //   {
  //     BodyMeshComponent->SetRelativeScale3D(FVector(Settings.MeshScale));
  //   }
  // }

  AddCameraLocation(Settings.CameraSettings.CameraOffset);
}

bool AAvatarPawn::CheckCustomCamera(APlayerController* NewController)
{
  if (CustomCamera == nullptr || NewController == nullptr)
  {
    return false;
  }

  NewController->SetViewTarget(CustomCamera);
  CameraInitialLocation = NewController->GetViewTarget()->GetActorLocation();

  return true;
}

void AAvatarPawn::AddCameraLocation(const FVector& Location)
{
  if (GetAvatarsController())
  {
    AActor* Camera = AvatarsController->GetViewTarget();
    if (Camera != nullptr)
    {
      FVector ClampedLocation;
      ClampedLocation.X = FMath::Clamp(Location.X, -CameraMaxOffset.X, CameraMaxOffset.X);
      ClampedLocation.Y = FMath::Clamp(Location.Y, -CameraMaxOffset.Y, CameraMaxOffset.Y);
      ClampedLocation.Z = FMath::Clamp(Location.Z, -CameraMaxOffset.Z, CameraMaxOffset.Z);
      Camera->SetActorLocation(CameraInitialLocation + ClampedLocation);
    }
  }
}

void AAvatarPawn::OnCameraMoveInput(const FVector2D& Movement)
{
  CameraOffset += FVector(Movement.Y, 0.0, Movement.X);
  AddCameraLocation(CameraOffset);
}

void AAvatarPawn::OnCameraZoomInput(const float Zoom)
{
  CameraOffset.Y += Zoom;
  AddCameraLocation(CameraOffset);
}

void AAvatarPawn::SetCameraZoomByAlpha(const float Alpha)
{
  if (GetAvatarsController())
  {
    AActor* Camera = AvatarsController->GetViewTarget();
    if (Camera != nullptr)
    {
      CameraOffset.Y = FMath::GetMappedRangeValueClamped(FVector2D(0.0, 1.0), FVector2D(CameraMaxOffset.Y, -CameraMaxOffset.Y), Alpha);
      Camera->SetActorLocation(CameraInitialLocation + CameraOffset);
    }
  }
}

void AAvatarPawn::ResetCameraLocation()
{
  CameraOffset = FVector::ZeroVector;
  AddCameraLocation(CameraOffset);
}

void AAvatarPawn::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

EAvatarState AAvatarPawn::GetState()
{
  return AvatarState;
}

FString AAvatarPawn::MapStateToString(EAvatarState State)
{
  switch (State)
  {
  case EAvatarState::Idle:
    return "Idle";
  case EAvatarState::Listening:
    return "Listening";
  case EAvatarState::Thinking:
    return "Thinking";
  case EAvatarState::Talking:
    return "Talking";
  default:
    return "Unknown";
  }
}

bool AAvatarPawn::IsInState(EAvatarState State)
{
  return AvatarState == State;
}

void AAvatarPawn::SetState(EAvatarState NewState)
{
  EAvatarState OldState = AvatarState;
  AvatarState = NewState;
  if (OnAvatarStateChangeEvent.IsBound())
  {
    OnAvatarStateChangeEvent.Broadcast(OldState, NewState);
  }
  if (bDebug) ULog::InfoIf(GetAvatarsController(), "Avatar state changed to: " + MapStateToString(NewState));

  UBodyAnimInstance* BodyAnimBP = Cast<UBodyAnimInstance>(FaceMeshComponent->GetAnimInstance());
  if (BodyAnimBP == nullptr)
  {
    if (bDebug) ULog::Error("Could not get reference to UBodyAnimInstance* BodyAnimBP");
    return;
  }
  BodyAnimBP->SetAvatarState(NewState);
}

bool AAvatarPawn::GetDialogSoundWave(const FString Path, const FString AssetName, FString LanguageString, USoundWave*& OutDialogSound)
{
  const FString AudioPath = Path + "Dialogs/" + LanguageString + "/Audio/";

  if (LastDialogPath == (AudioPath + AssetName) && LastDialogSoundWave != nullptr)
  {
    OutDialogSound = LastDialogSoundWave;
    return true;
  }

  LastDialogSoundWave = FGet::GetAssetByPath<USoundWave>(AudioPath, AssetName);

  if (LastDialogSoundWave == nullptr)
  {
    if (bDebug)
      ULog::Error("Could not get reference to USoundWave: " + AssetName + " for path: " + AudioPath + "in object: " + this->GetName());

    return false;
  }

  OutDialogSound = LastDialogSoundWave;
  return true;
}

void AAvatarPawn::PlayDialogSoundWave(USoundWave* DialogSound)
{
  if (DialogSound != nullptr)
  {
    AudioComponent->Stop();
    AudioComponent->SetSound(DialogSound);
    AudioComponent->Play();
  }
}

void AAvatarPawn::OnAudioFinishedPlaying()
{
  // TODO: find a way to unload sound from memory instead of destroying it entirely
  // (which also corrupts audio files in editor)

  // USoundBase* SoundToUnload = AudioComponent->Sound;
  // if (SoundToUnload == nullptr)
  // {
  //   return;
  // }
  // SoundToUnload->ConditionalBeginDestroy();

  UFaceAnimInstance* FaceAnimBP = Cast<UFaceAnimInstance>(FaceMeshComponent->GetAnimInstance());
  if (FaceAnimBP == nullptr)
  {
    if (bDebug) ULog::Error("Could not get reference to UFaceAnimInstance* FaceAnimBP");
    return;
  }
  FaceAnimBP->StopFaceAnimation();
  if (IsInState(EAvatarState::Talking)) SetState(EAvatarState::Idle);
}

bool AAvatarPawn::GetDialogFaceAnimation(
    const FString Path, const FString AssetName, FString LanguageString, UAnimSequence*& OutFaceAnimation
)
{
  const FString AnimationPath = Path + "Dialogs/" + LanguageString + "/Animations/";
  const FString FullName = TEXT("animFace_") + AssetName;

  if (LastFaceAnimationPath == (AnimationPath + FullName) && LastFaceAnimation != nullptr)
  {
    OutFaceAnimation = LastFaceAnimation;
    return true;
  }

  LastFaceAnimation = FGet::GetAssetByPath<UAnimSequence>(AnimationPath, FullName);

  if (LastFaceAnimation == nullptr)
  {
    if (bDebug)
      ULog::Error(
          "Could not get reference to UAnimSequence: " + FullName + " for path: " + AnimationPath + "in object: " + this->GetName()
      );
    return false;
  }

  LastFaceAnimationPath = AnimationPath + FullName;

  OutFaceAnimation = LastFaceAnimation;
  return true;
}

void AAvatarPawn::PlayDialogFaceAnimation(UAnimSequence* FaceAnimation)
{
  UFaceAnimInstance* FaceAnimBP = Cast<UFaceAnimInstance>(FaceMeshComponent->GetAnimInstance());

  if (FaceAnimBP == nullptr)
  {
    if (bDebug) ULog::Error("Could not get reference to UFaceAnimInstance* FaceAnimBP");
    return;
  };

  FaceAnimBP->PlayFaceAnimation(FaceAnimation);
}

void AAvatarPawn::PlayDialog(const FString Path, const FString AssetName, FString LanguageString)
{
  UAnimSequence* Animation = nullptr;
  GetDialogFaceAnimation(Path, AssetName, "Common", Animation) || GetDialogFaceAnimation(Path, AssetName, LanguageString, Animation);

  USoundWave* SoundWave = nullptr;
  GetDialogSoundWave(Path, AssetName, "Common", SoundWave) || GetDialogSoundWave(Path, AssetName, LanguageString, SoundWave);

  if (!SoundWave || !Animation) return;

  PlayDialogFaceAnimation(Animation);
  PlayDialogSoundWave(SoundWave);
  SetState(EAvatarState::Talking);
}

void AAvatarPawn::StopDialog()
{
  AudioComponent->FadeOut(1.f, 0.f, EAudioFaderCurve::Logarithmic);
  UFaceAnimInstance* FaceAnimBP = Cast<UFaceAnimInstance>(FaceMeshComponent->GetAnimInstance());

  if (FaceAnimBP == nullptr)
  {
    if (bDebug) ULog::Error("Could not get reference to UFaceAnimInstance* FaceAnimBP");
    return;
  };

  FaceAnimBP->StopFaceAnimation();
  SetState(EAvatarState::Idle);
}

void AAvatarPawn::ApplySceneSetup()
{
  SetActorLocation(SceneSetup.Location);
  SetActorRotation(SceneSetup.Rotation);

  if (BodyMeshComponent != nullptr)
  {
    BodyMeshComponent->SetRelativeScale3D(FVector(SceneSetup.MeshScale));
  }
}
