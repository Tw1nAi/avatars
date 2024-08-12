// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "Animation/AnimSequence.h"
#include "CoreMinimal.h"
#include "Sound/SoundWave.h"

#include "Get.h"

#include "MultiPartDialog.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FMultiPartDialog
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector2D PartsDelayRange = FVector2D(1.0f, 1.5f);
  UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<USoundWave*> AudioFiles;
  UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<UAnimSequence*> AnimationFiles;
  UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Path;
  UPROPERTY(EditAnywhere, BlueprintReadWrite) FString LanguageString;
  UPROPERTY(EditAnywhere, BlueprintReadWrite) FString AssetName;
  UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 CurrentPart = -1;

  bool IsPlaying() const { return CurrentPart > -1 && AudioFiles.Num() > 0 && AudioFiles.Num() == AnimationFiles.Num(); }

  bool GetAudioFiles()
  {
    const FString AudioPath = "/Game" + Path + "Dialogs/" + LanguageString + "/Audio/" + AssetName + "/";
    UE_LOG(LogTemp, Warning, TEXT("Searching for multi part dialog audio in directory: %s"), *AudioPath);
    AudioFiles = FGet::GetAllAssetsByPath<USoundWave>(AudioPath);
    if (AudioFiles.Num() < 1)
    {
      FString Message = "Could not find any audio files in: " + AudioPath;
      UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
      return false;
    }

    AudioFiles.StableSort([](const USoundWave& A, const USoundWave& B) { return A.GetName() < B.GetName(); });
    return true;
  }

  bool GetAnimationsFiles()
  {
    const FString AnimationsPath = "/Game" + Path + "Dialogs/" + LanguageString + "/Animations/" + AssetName + "/";
    UE_LOG(LogTemp, Warning, TEXT("Searching for multi part dialog animations in directory: %s"), *AnimationsPath);
    AnimationFiles = FGet::GetAllAssetsByPath<UAnimSequence>(AnimationsPath);
    if (AnimationFiles.Num() < 1)
    {
      FString Message = "Could not find any animation files in: " + AnimationsPath;
      UE_LOG(LogTemp, Error, TEXT("%s"), *Message);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
      return false;
    }

    AnimationFiles.StableSort([](const UAnimSequence& A, const UAnimSequence& B) { return A.GetName() < B.GetName(); });
    return true;
  }

  bool Setup(const FString NewPath, const FString NewAssetName, FString NewLanguageString)
  {
    // TODO: add check if the dialog was played previously and reuse data instead of setting it up again

    Path = NewPath;
    AssetName = NewAssetName;
    LanguageString = NewLanguageString;
    if (!GetAudioFiles() || !GetAnimationsFiles()) return false;
    if (AudioFiles.Num() != AnimationFiles.Num())
    {
      UE_LOG(LogTemp, Error, TEXT("The number of audio and animation files do not match."));
      return false;
    }
    CurrentPart = 0;
    return true;
  }

  bool SetNextPart()
  {
    if (!IsPlaying())
    {
      UE_LOG(LogTemp, Error, TEXT("Can't set next part of the multi part dialog as it is not playing."));
      return false;
    }

    if (CurrentPart == AudioFiles.Num() - 1)
    {
      UE_LOG(LogTemp, Warning, TEXT("Can not set next part of the multi part dialog as it played all parts."));
      return false;
    }

    CurrentPart++;
    return true;
  }

  USoundWave* GetNextAudioFile()
  {
    if (CurrentPart < 0 || CurrentPart >= AudioFiles.Num())
    {
      UE_LOG(LogTemp, Error, TEXT("Current part index is out of range."));
      return nullptr;
    }
    return AudioFiles[CurrentPart];
  }

  UAnimSequence* GetNextAnimationFile()
  {
    if (CurrentPart < 0 || CurrentPart >= AnimationFiles.Num())
    {
      UE_LOG(LogTemp, Error, TEXT("Current part index is out of range."));
      return nullptr;
    }
    return AnimationFiles[CurrentPart];
  }

  float GetPartDelay() const
  {
    if (CurrentPart < 0 || CurrentPart >= AudioFiles.Num())
    {
      UE_LOG(LogTemp, Error, TEXT("Current part index is out of range."));
      return 0.0f;
    }
    return FMath::RandRange(PartsDelayRange.X, PartsDelayRange.Y);
  }

  void Stop()
  {
    CurrentPart = -1;
    AudioFiles.Empty();
    AnimationFiles.Empty();
  }
};