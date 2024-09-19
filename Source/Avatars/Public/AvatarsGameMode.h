// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Misc/Paths.h"
#include "Templates/SharedPointer.h"

#include "AvatarsGameMode.generated.h"

UCLASS()
class AVATARS_API AAvatarsGameMode : public AGameModeBase
{
  GENERATED_BODY()

public:
  UFUNCTION(BlueprintCallable)
  FString ExecuteProcess(const FString& Path, const FString& Params)
  {
    UE_LOG(LogTemp, Warning, TEXT("Attempting to execute process on path: %s"), *Path);
    FString Output;
    FString Errors;

    // Execute the command
    int32* OutReturnCode = nullptr;
    const TCHAR* OptionalWorkingDirectory = nullptr;
    const bool bShouldEndWithParentProcess = true;
    bool bSuccess = FPlatformProcess::ExecProcess(
        *Path,
        *Params,
        OutReturnCode,
        &Output,
        &Errors,
        OptionalWorkingDirectory,
        // *Path,
        bShouldEndWithParentProcess
    );

    if (!bSuccess)
    {
      // // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, *Errors);
      UE_LOG(LogTemp, Warning, TEXT("Could not execute process in path: %s due to error: %s"), *Path, *Errors);
      UE_LOG(LogTemp, Warning, TEXT("Output: %s"), *Output);
      return Errors;
    }

    // // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, *Errors);
    UE_LOG(LogTemp, Warning, TEXT("Succesfully executed process in path: %s, OutReturnCode: %d"), *Path, OutReturnCode);
    UE_LOG(LogTemp, Warning, TEXT("Output: %s"), *Output);
    UE_LOG(LogTemp, Warning, TEXT("Errors: %s"), *Errors);
    return Output;
  }

  /* Returns the directory the application was launched from. */
  UFUNCTION(BlueprintCallable, BlueprintPure)
  FString GetLunchPath() { return FPaths::LaunchDir(); }

  /* Returns the directory the application was launched from. */
  UFUNCTION(BlueprintCallable, BlueprintPure)
  FString GetSourcePath() { return FPaths::GameSourceDir(); }

  /* Returns the directory the application was launched from. */
  UFUNCTION(BlueprintCallable, BlueprintPure)
  FString ProjectDir()
  {
    FString ProjectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
    // // GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, *ProjectDir);
    UE_LOG(LogTemp, Log, TEXT("Project Directory: %s"), *ProjectDir);
    return ProjectDir;
  }
};
