// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ComboBoxString.h"
#include "CoreMinimal.h"
#include "WidgetBase.h"

#include "ComboBoxStringWidgetBase.generated.h"

/**
 *
 */
UCLASS()
class WIDGETSTOOLKIT_API UComboBoxStringWidgetBase : public UWidgetBase
{
  GENERATED_BODY()

public:
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
  TObjectPtr<UComboBoxString> ComboBox;
};
