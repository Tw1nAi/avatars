// Copyright Juice sp. z o. o., All Rights Reserved.

#include "RestApi/RestApi.h"

URestApi::URestApi()
{
  AddDefaultHeader("User-Agent", "X-UnrealEngine-Agent");
}

void URestApi::SetBaseUrl(const FString InBaseUrl)
{
  BaseUrl = InBaseUrl;
}

void URestApi::AddDefaultHeader(const FString Name, const FString Value)
{
  FHttpHeader Header;
  Header.Name = Name;
  Header.Value = Value;
  DefaultHeaders.Emplace(Header);
}

void URestApi::OnError(FString ErrorMessasge = FString(TEXT("Api Error")))
{
  // TODO: handle errors
  if (!bDebug) return;

  GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, ErrorMessasge);
  UE_LOG(LogTemp, Warning, TEXT("%s"), *ErrorMessasge);
  FDebug::DumpStackTraceToLog(ELogVerbosity::Type::Error);
}
