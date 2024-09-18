#pragma once

#include "AnimGraphNode_Base.h"
#include "AnimNode_CustomRandomPlayer.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

#include "AnimGraphNode_CustomRandomPlayer.generated.h"

UCLASS(MinimalAPI)
class UAnimGraphNode_CustomRandomPlayer : public UAnimGraphNode_Base
{
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, Category = Settings)
  FAnimNode_CustomRandomPlayer Node;

  // UEdGraphNode interface
  virtual FLinearColor GetNodeTitleColor() const override;
  virtual FText GetTooltipText() const override;
  virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
  virtual FText GetMenuCategory() const override;
  // End of UEdGraphNode interface

  // UAnimGraphNode_Base interface
  virtual void ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog) override;
  virtual void CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode) override;
  virtual void GetOutputLinkAttributes(FNodeAttributeArray& OutAttributes) const override;
  // End of UAnimGraphNode_Base interface

private:
  virtual void CreateOutputPins() override;
};
