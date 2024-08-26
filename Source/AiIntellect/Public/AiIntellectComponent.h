#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "AiIdentityInterface.h"

#include "AiIntellectComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAiIntellect, Display, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
  FAiResponseSignature, FString, Message, FString, AudioPath, const TArray<FString>&, NewSuggestions, const TArray<FString>&, Tags
);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AIINTELLECT_API UAiIntellectComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  UAiIntellectComponent();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FAiIdentity Identity;

  UFUNCTION(BlueprintCallable)
  bool GetOwnerIdentity();

  FAiResponseSignature OnAiResponseEvent;

  /* Return false if could not send the message. */
  virtual bool RespondTo(const FString& Message = FString(), const FString& LanguageIso = FString());
  virtual void StartNewConversation(const FString& LanguageIso = FString());
  virtual bool HasStartedConversation() const;

  virtual void BeginPlay() override;
};
