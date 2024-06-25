#include "AiIntellectComponent.h"

// Sets default values for this component's properties
UAiIntellectComponent::UAiIntellectComponent()
{
  PrimaryComponentTick.bCanEverTick = false;
}

void UAiIntellectComponent::BeginPlay()
{
  Super::BeginPlay();

  if (!IntellectClass)
  {
    UE_LOG(LogTemp, Error, TEXT("Intellect class is not set"));
    return;
  }

  UObject* IntellectObject = NewObject<UObject>(this, IntellectClass);
  if (IntellectObject && IntellectObject->GetClass()->ImplementsInterface(UAiIntellectInterface::StaticClass()))
  {
    Intellect.SetInterface(Cast<IAiIntellectInterface>(IntellectObject));
    Intellect.SetObject(IntellectObject);
  }
  else
  {
    UE_LOG(LogTemp, Error, TEXT("Intellect class does not implement IAiIntellectInterface"));
  }
}
