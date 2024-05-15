// Copyright Juice sp. z o. o., All Rights Reserved.

#pragma once

#include "AvatarsTypes.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "AvatarApiComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetAvatarsDelegate, const TArray<FAvatarData>&, Response);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAvatarResponseDelegate, FText, Message);

class UAvatarsRestApi;

/*
  The Avatar API component is a base class for all Avatar AI API versions. It provides a common
  interface for sending user messages to the server and receiving responses from AI services. The
  base class should not implement any details, only provide a common interface.
*/
UCLASS(ClassGroup = (Avatars), Abstract, BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class AVATARS_API UAvatarApiComponent : public UActorComponent
{
  GENERATED_BODY()

public:
  UAvatarApiComponent();

  FOnGetAvatarsDelegate OnGetAvatarsEvent;
  FOnAvatarResponseDelegate OnAvatarResponseEvent;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars API Services")
  bool bDebug = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatars API Services")
  bool bShowRuntimeProperties = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString BaseUrl;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "65535", UIMin = "0", UIMax = "65535"))
  int32 Port;

  UPROPERTY(EditAnywhere, BlueprintReadWrite)
  FString UrlPath;

  /* Remove leading and trailing slashes */
  void CleanUrl(FString& InOutUrl) const;

  UFUNCTION(BlueprintCallable)
  FString GetFullUrl() const;

  UFUNCTION(BlueprintCallable)
  bool CheckAndFormatMessage(FString& InOutMessage) const;

  UPROPERTY(
      VisibleAnywhere,
      BlueprintReadWrite,
      Category = "Avatars API Services",
      meta = (EditCondition = "bShowRuntimeProperties", EditConditionHides)
  )
  TArray<FAvatarData> Avatars;

  /* Call API do get all avilable avatars with their Ids, names, etc. */
  virtual void GetAvatars();

  /* Initiate new conversation for selected avatar. */
  virtual void StartNewConversation(const EAvatarLanguage Language, const uint8 AvatarId);

  /* Receive user message as a string and use Avatar API to send this to the AI model for
   * processing. */
  virtual void SendUserMessage(FString Message);

protected:
  virtual void BeginPlay() override;
};
