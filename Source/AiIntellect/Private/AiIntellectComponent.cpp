#include "AiIntellectComponent.h"

DEFINE_LOG_CATEGORY(LogAiIntellect);

// Sets default values for this component's properties
UAiIntellectComponent::UAiIntellectComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

bool UAiIntellectComponent::GetOwnerIdentity()
{
  AActor* ComponentOwner = GetOwner();
  if (ComponentOwner == nullptr)
  {
    FString WarningMessage = FString::Printf(TEXT("ComponentOwner is not valid for %s in UAiIntellectComponent::GetOwnerIdentity"), *GetOwner()->GetName());
    UE_LOG(LogAiIntellect, Warning, TEXT("%s"), *WarningMessage);
    // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, WarningMessage);
    return false;
  }

  IAiIdentityInterface* OwnerWithIdentity = Cast<IAiIdentityInterface>(ComponentOwner);
  if (OwnerWithIdentity == nullptr)
  {
    FString WarningMessage = FString::Printf(TEXT("OwnerWithIdentity is not valid for %s in UAiIntellectComponent::GetOwnerIdentity"), *GetOwner()->GetName());
    UE_LOG(LogAiIntellect, Warning, TEXT("%s"), *WarningMessage);
    // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, WarningMessage);
    return false;
  }

  Identity = OwnerWithIdentity->GetIdentity();
  if (!Identity.IsValid())
  {
    FString ErrorMessage = FString::Printf(TEXT("AvatarData is not valid for %s in UAiIntellectComponent::GetOwnerIdentity"), *GetOwner()->GetName());
    UE_LOG(LogAiIntellect, Error, TEXT("%s"), *ErrorMessage);
    // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ErrorMessage);
    return false;
  }

  return true;
}

bool UAiIntellectComponent::RespondTo(const FString& Message, const FString& LanguageIso)
{
  return false;
}

void UAiIntellectComponent::StartNewConversation(const FString& LanguageIso)
{
  return;
}

bool UAiIntellectComponent::HasStartedConversation() const
{
  return false;
}

void UAiIntellectComponent::BeginPlay()
{
  Super::BeginPlay();

  GetOwnerIdentity();
}
