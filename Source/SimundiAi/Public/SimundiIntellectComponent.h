#pragma once

#include "CoreMinimal.h"

#include "AiIntellectComponent.h"
#include "Api/Models/Models.h"

#include "AiIdentity.h"

#include "SimundiIntellectComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSimundiIntellect, Display, All);

class USimundiApi;

USTRUCT(BlueprintType, Blueprintable)
struct FSimundiIntellectSettings
{
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString BaseUrl = FString(TEXT("http://localhost:5005/api/v2"));
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIMUNDIAI_API USimundiIntellectComponent : public UAiIntellectComponent
{
  GENERATED_BODY()

protected:
  UPROPERTY()
  TObjectPtr<USimundiApi> Api;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
  FString AvatarId;

  // This messages list allows to delay sending messages from user to AI until the conversation is started.
  UPROPERTY()
  FString PendingMessage;

  UPROPERTY()
  FString ConversationId;

  void SetConversationId(const FString Id);

  void GetAvatarsData();

public:
  USimundiIntellectComponent();

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FSimundiIntellectSettings Settings;

  virtual bool RespondTo(const FString& Message = FString(), const FString& LanguageIso = FString()) override;
  virtual void StartNewConversation(const FString& LanguageIso = FString()) override;
  virtual bool HasStartedConversation() const override;

  virtual void BeginPlay() override;
};
