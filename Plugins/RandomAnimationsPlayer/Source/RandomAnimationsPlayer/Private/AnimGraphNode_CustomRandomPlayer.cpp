#include "AnimGraphNode_CustomRandomPlayer.h"

#include "Animation/AnimAttributes.h"
#include "Animation/AnimRootMotionProvider.h"
#include "AnimationGraphSchema.h"
#include "EditorCategoryUtils.h"

#define LOCTEXT_NAMESPACE "AnimGraphNode_CustomRandomPlayer"

FLinearColor UAnimGraphNode_CustomRandomPlayer::GetNodeTitleColor() const
{
  return FLinearColor(0.10f, 0.60f, 0.12f);
}

FText UAnimGraphNode_CustomRandomPlayer::GetTooltipText() const
{
  return LOCTEXT("NodeToolTip", "Plays sequences picked from a provided list in random orders.");
}

FText UAnimGraphNode_CustomRandomPlayer::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
  return LOCTEXT("NodeTitle", "Custom Random Sequence Player");
}

FText UAnimGraphNode_CustomRandomPlayer::GetMenuCategory() const
{
  return LOCTEXT("NodeCategory", "Animation|Sequences");
}

void UAnimGraphNode_CustomRandomPlayer::GetOutputLinkAttributes(FNodeAttributeArray& OutAttributes) const
{
  OutAttributes.Add(UE::Anim::FAttributes::Curves);
  OutAttributes.Add(UE::Anim::FAttributes::Attributes);

  if (UE::Anim::IAnimRootMotionProvider::Get())
  {
    OutAttributes.Add(UE::Anim::IAnimRootMotionProvider::AttributeName);
  }
}

void UAnimGraphNode_CustomRandomPlayer::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog)
{
  if (Node.Entries.Num() == 0)
  {
    MessageLog.Warning(*LOCTEXT("UAnimGraphNode_CustomRandomPlayer_EmptyEntriesWarning", "@@ - No entries where added. Please add entries.").ToString(), this);
  }
}

void UAnimGraphNode_CustomRandomPlayer::CopyNodeDataToPreviewNode(FAnimNode_Base* InPreviewNode)
{
  FAnimNode_CustomRandomPlayer* InNode = static_cast<FAnimNode_CustomRandomPlayer*>(InPreviewNode);

  InNode->Entries = Node.Entries;
}

void UAnimGraphNode_CustomRandomPlayer::CreateOutputPins()
{
  const UAnimationGraphSchema* Schema = GetDefault<UAnimationGraphSchema>();

  FCreatePinParams PinParams = FCreatePinParams();
  PinParams.bIsConst = false;
  PinParams.bIsReference = true;

  // What do I do here?
  CreatePin(EGPD_Input, Schema->PC_Struct, TEXT("Entries"), TBaseStructure<TArray<FRandomPlayerSequenceEntryCustom>>::Get(), PinParams);
}

#undef LOCTEXT_NAMESPACE
