// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Camera/CameraActor.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstance.h"

#include "AiIdentityInterface.h"
#include "AvatarsTypes.h"
#include "Dialogs/MultiPartDialog.h"
#include "Settings/Settings.h"

#include "AvatarPawn.generated.h"

class UAiIntellectComponent;
class AAvatarsPlayerController;
class APlayerController;
class UAudioStreamComponent;
class FAssetRegistryModule;
class UAudioComponent;
class USceneComponent;
class USkeletalMeshComponent;
class USoundWave;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAvatarStateChangeSignature, EAvatarState, OldState, EAvatarState, NewState);

/* Avatar pawn is a base class for all avatars in the game. It provides basic functionality for managing avatar's state, camera and dialog.
 */
UCLASS(Blueprintable, BlueprintType)
class AVATARS_API AAvatarPawn : public APawn, public IAiIdentityInterface
{
  GENERATED_BODY()

public:
  AAvatarPawn();

  /* If set above 0 (zero) it will be used to place avatar in order from smallest to the biggest order value. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
  int32 Order = 0;

  /* Use this to exclude avatars in the scene from being displayed. The characters that have bEnabled set to "false" will be removed from the scene on the controller's BeginPlay()
   * call. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
  bool bEnabled = true;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar")
  TObjectPtr<UAiIntellectComponent> IntellectComponent;

  /* Use this property to set reference for custom camera for this actor.
    If not null it will be used as camera fot this avatar upon selection/possession. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
  TObjectPtr<ACameraActor> CustomCamera;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
  bool bDebug;

protected:
  AAvatarsPlayerController* AvatarsController;
  bool GetAvatarsController();

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
  FAiIdentity AvatarData;

  virtual FAiIdentity& GetIdentity() override;

  UFUNCTION(BlueprintCallable)
  void ApplySettings(FAvatarSettings& Settings);

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  USceneComponent* SceneComponent;

  // TODO: could be instanced any component that inherits from the AiVoice interface, which would allow for more generic approach.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UAudioComponent* AudioComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  USkeletalMeshComponent* BodyMeshComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  USkeletalMeshComponent* FaceMeshComponent;

  // TODO: could be instanced of any component that inherits from AiHearing interface, which would allow for more generic approach.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UAudioStreamComponent* AudioStreamComponent;

  // TODO: both below methods should be placed within the AiVoice interface as StartListening and StopListening.
  /* Use this to initiate avatar's listening services, e.g. audio stream or other. */
  UFUNCTION(BlueprintCallable)
  void StartAudioStream();

  UFUNCTION(BlueprintCallable)
  void StopAudioStream();

protected:
  /* This state should stay protected to allow for side effect when setting it via SetState() method. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
  EAvatarState AvatarState;

public:
  UFUNCTION(BlueprintCallable)
  FString MapStateToString(EAvatarState State);

  UFUNCTION(BlueprintCallable)
  bool IsInState(EAvatarState State);

  UFUNCTION(BlueprintCallable, BlueprintPure)
  EAvatarState GetState();

  FAvatarStateChangeSignature OnAvatarStateChangeEvent;

  UFUNCTION(BlueprintCallable)
  void SetState(EAvatarState NewState);

  UFUNCTION(BlueprintCallable)
  bool GetDialogSoundWave(const FString Path, const FString AssetName, FString LanguageString, USoundWave*& OutDialogSound);

  UPROPERTY()
  USoundWave* LastDialogSoundWave;
  FString LastDialogPath;

  UFUNCTION(BlueprintCallable)
  void PlayDialogSoundWave(USoundWave* DialogSound);

  UFUNCTION(BlueprintCallable)
  bool GetDialogFaceAnimation(const FString Path, const FString AssetName, FString LanguageString, UAnimSequence*& OutFaceAnimation);

  UPROPERTY()
  UAnimSequence* LastFaceAnimation;
  FString LastFaceAnimationPath;

  UFUNCTION(BlueprintCallable)
  void PlayDialogFaceAnimation(UAnimSequence* FaceAnimation);

  UFUNCTION(BlueprintCallable)
  void PlayDialog(const FString Path, const FString AssetName, FString LanguageString, const bool bIsMultiPartDialog = false);

  UFUNCTION(BlueprintCallable)
  void StopDialog();

  UFUNCTION(BlueprintCallable)
  void OnAudioFinishedPlaying();

  UPROPERTY()
  FMultiPartDialog MultiPartDialog;

  /* Maximum offset allowed for user to make. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar")
  FVector CameraMaxOffset;

  /* Overall offset used with AddCameraLocation, the axes are:
    X = right (+), left (-)
    y = far (+), close (-)
    z = up (+), down (-)
   */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar")
  FVector CameraInitialLocation;

  /* Check if avatar has its custom camera actor reference set and stet that camera to active. Return false if no custom camera was found.
   */
  bool CheckCustomCamera(APlayerController* NewController) const;

  /* Use value between 0 and 1 to scale zoom between min and max values/ */
  UFUNCTION(BlueprintCallable)
  void SetCameraZoomByAlpha(const float Alpha);

  UFUNCTION(BlueprintCallable)
  void OnCameraZoomInput(const float Zoom);

  UFUNCTION(BlueprintCallable)
  void OnCameraMoveInput(const FVector2D& Movement);

  UFUNCTION(BlueprintCallable)
  void AddCameraLocation(const FVector& Location);

  UFUNCTION(BlueprintCallable)
  void ResetCameraLocation();

  FVector CameraOffset;

protected:
  virtual void BeginPlay() override;

public:
  virtual void Tick(float DeltaTime) override;
};
