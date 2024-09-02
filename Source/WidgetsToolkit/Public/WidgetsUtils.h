// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#if WITH_EDITORONLY_DATA
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "Editor/UMGEditor/Public/WidgetBlueprint.h"
#endif

#include "WidgetsUtils.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWidgetsUtils, Log, All);

UCLASS()
class WIDGETSTOOLKIT_API UWidgetsUtils : public UObject
{
  GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
  /* Mind that this class should be only use with #if WITH_EDITOR macro, since it's flow uses classes that are not available in runtime */
  template <typename T> static T* CreateEditorWidget(UObject* Outer, const TSubclassOf<UWidget> WidgetClass, const FName WidgetName)
  {
    const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(Outer->GetClass());
    // or const UWidgetBlueprintGeneratedClass* WidgetBlueprintGeneratedClass = Cast<UWidgetBlueprintGeneratedClass>(WidgetClass->ClassGeneratedBy);

    if (!WidgetBlueprintGeneratedClass)
    {
      const FString Message = FString::Printf(TEXT("Failed to get outer class widget for class %s"), *WidgetClass->GetName());
      UE_LOG(LogWidgetsUtils, Error, TEXT("%s"), *Message);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);

      return nullptr;
    };

    const UPackage* Package = WidgetBlueprintGeneratedClass->GetPackage();
    // or UPackage* Package = WidgetBlueprintGeneratedClass->GetOuter()->GetOutermost();
    if (!Package)
    {
      const FString Message = FString::Printf(TEXT("Failed to create get package while creating editor widget for class %s"), *WidgetClass->GetName());
      UE_LOG(LogWidgetsUtils, Error, TEXT("%s"), *Message);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);
      return nullptr;
    };

    const UWidgetBlueprint* MainAsset = Cast<UWidgetBlueprint>(Package->FindAssetInPackage());
    if (!MainAsset)
    {
      const FString Message = FString::Printf(TEXT("Failed to create get main asset while creating editor widget for class %s"), *WidgetClass->GetName());
      UE_LOG(LogWidgetsUtils, Error, TEXT("%s"), *Message);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);

      return nullptr;
    };

    UWidget* BaseWidget = MainAsset->WidgetTree->ConstructWidget<T>(WidgetClass, WidgetName);
    if (!BaseWidget)
    {
      const FString Message = FString::Printf(TEXT("Failed to create editor widget for class %s"), *WidgetClass->GetName());
      UE_LOG(LogWidgetsUtils, Error, TEXT("%s"), *Message);
      GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Message);

      return nullptr;
    }

    return Cast<T>(BaseWidget);
  }
#endif
};