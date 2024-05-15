// Juice Sp. z o. o.. All rights reserved.

#pragma once

#include "Engine/DataTable.h"

#include "Translations.generated.h"

USTRUCT(BlueprintType)
struct AVATARS_API FTranslationsTableRow : public FTableRowBase
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
  FText PL;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
  FText EN;
};