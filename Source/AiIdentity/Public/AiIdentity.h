#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "AiIdentity.generated.h"

/* A single structure to uniquely identify AI persona. Allow this to be a data table */
USTRUCT(BlueprintType)
struct AIIDENTITY_API FAiIdentity : public FTableRowBase
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString Name;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString AssetsPath;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  UMaterialInstance* ThumbnailImage;

  bool IsValid() const { return !Name.IsEmpty() && !AssetsPath.IsEmpty() && ThumbnailImage != nullptr; }

  // Match incoming name by splitting it at spaces and checking them against own name parts.
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