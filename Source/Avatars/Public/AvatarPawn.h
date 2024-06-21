// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Camera/CameraActor.h"
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInstance.h"

#include "AvatarsTypes.h"
#include "Settings/Settings.h"

#include "AvatarPawn.generated.h"

class AAvatarsPlayerController;
class APlayerController;
class UAudioStreamComponent;
class FAssetRegistryModule;
class UAudioComponent;
class UAvatarsApiBase;
class USceneComponent;
class USkeletalMeshComponent;
class USoundWave;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAvatarStateChangeSignature, EAvatarState, OldState, EAvatarState, NewState);

USTRUCT(BlueprintType)
struct AVATARS_API FAvatarData : public FTableRowBase
{
  GENERATED_BODY()

public:
  FAvatarId Id;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EAvatarCharacter AvatarTag;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Name;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString AssetsPath;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EApiVersion ApiVersion;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TSubclassOf<AAvatarPawn> AvatarClass;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UMaterialInstance* ThumbnailImage;

  // Match incomming name by splitting it on spaces and checking all parts are maching with own splited name.
  bool MatchByName(const FString& NameToMatch) const
  {
    TArray<FString> NameParts;
    NameToMatch.ParseIntoArray(NameParts, TEXT(" "), true);
    TArray<FString> OwnNameParts;
    Name.ParseIntoArray(OwnNameParts, TEXT(" "), true);

    if (NameParts.Num() != OwnNameParts.Num()) return false;

    for (int32 i = 0; i < NameParts.Num(); i++)
    {
      if (NameParts[i] != OwnNameParts[i]) return false;
    }

    return true;
  }
};

USTRUCT(BlueprintType)
struct AVATARS_API FAvatarSceneSetup
{
  GENERATED_BODY()

public:
  /* The location the avatar will placed to both at in the construction script and at the begin play. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector Location = FVector(0.0, 0.0, -3.5);

  /* Same as location but for avatar's rotation. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FRotator Rotation;

  /* Single float to control the value of lights at the scene. Value of 1 (one) is the one set by default in editor in level. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float LightsIntensity = 1.0;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  float MeshScale = 1.0;
};

UCLASS()
class AVATARS_API AAvatarPawn : public APawn
{
  GENERATED_BODY()

public:
  AAvatarPawn();

  /* Use this property to set reference for custom camera for this actor.
    If not null it will be used as camera fot this avatar upon selection/possesion. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  ACameraActor* CustomCamera;

  /* The version of the api that this avatar should use. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  TSubclassOf<UAvatarsApiBase> ApiClass;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bDebug;

protected:
  AAvatarsPlayerController* AvatarsController;
  bool GetAvatarsController();

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FAvatarId Id;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  EApiVersion ApiVersion;

  UFUNCTION(BlueprintCallable)
  void ApplySettings(FAvatarSettings& Settings);

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  USceneComponent* SceneComponent;

  // TODO: could be instance any component that inherits from the AiVoice interface, which would allow for more generic approach.
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UAudioComponent* AudioComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  USkeletalMeshComponent* BodyMeshComponent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  USkeletalMeshComponent* FaceMeshComponent;

  // TODO: could be instance of any component that inherits from AiHearing interface, which would allow for more generic approach.
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
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
  void PlayDialog(const FString Path, const FString AssetName, FString LanguageString);

  UFUNCTION(BlueprintCallable)
  void StopDialog();

  UFUNCTION(BlueprintCallable)
  void OnAudioFinishedPlaying();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  bool bApplySceneSetup;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FAvatarSceneSetup SceneSetup;

  UFUNCTION(BlueprintCallable)
  void ApplySceneSetup();

  /* Maximum offset allowed for user to make. */
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FVector CameraMaxOffset;

  /* Overall offset used with AddCameraLocation, the axes are:
    X = right (+), left (-)
    y = far (+), close (-)
    z = up (+), down (-)
   */
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FVector CameraInitialLocation;

  /* Check if avatar has its custom camera actor reference set and stet that camera to active. Return false if no custom camera was found.
   */
  bool CheckCustomCamera(APlayerController* NewController);

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
