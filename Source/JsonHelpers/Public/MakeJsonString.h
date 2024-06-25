#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "Serialization/JsonSerializer.h"

/* Example usage:
  FString jsonString = MakeJsonString(
      "strValue", "Works!",
      "intValue", 42,
      "boolValue", true,
      "doubleValue", 3.14,
      "stringArray", TArray<const char*>{"one", "two", "three"},
      "numberArray", TArray<double>{1.0, 2.0, 3.0}
  );

*/

// Include necessary headers
#include "Json.h"
#include "Serialization/JsonSerializer.h"

class MakeJsonString
{
public:
  template <typename... Args> MakeJsonString(Args... args)
  {
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    AddField(JsonObject, args...);

    // Directly create a clean, condensed JSON string
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
  }

  operator FString() const { return OutputString; }

private:
  FString OutputString;

  // Base case for recursion
  static void AddField(TSharedPtr<FJsonObject>& JsonObject) {}

  // Recursive variadic template function to add fields
  template <typename T, typename... Args> static void AddField(TSharedPtr<FJsonObject>& JsonObject, const char* Key, T Value, Args... args)
  {
    AddFieldByType(JsonObject, FString(UTF8_TO_TCHAR(Key)), Value);
    AddField(JsonObject, args...);
  }

  // Overloads for different types
  static void AddFieldByType(TSharedPtr<FJsonObject>& JsonObject, const FString& Key, const char* Value)
  {
    JsonObject->SetStringField(Key, FString(UTF8_TO_TCHAR(Value)));
  }

  static void AddFieldByType(TSharedPtr<FJsonObject>& JsonObject, const FString& Key, const FString& Value)
  {
    JsonObject->SetStringField(Key, Value);
  }

  static void AddFieldByType(TSharedPtr<FJsonObject>& JsonObject, const FString& Key, double Value)
  {
    JsonObject->SetNumberField(Key, Value);
  }

  static void AddFieldByType(TSharedPtr<FJsonObject>& JsonObject, const FString& Key, int32 Value)
  {
    JsonObject->SetNumberField(Key, static_cast<double>(Value));
  }

  static void AddFieldByType(TSharedPtr<FJsonObject>& JsonObject, const FString& Key, bool Value) { JsonObject->SetBoolField(Key, Value); }

  static void AddFieldByType(TSharedPtr<FJsonObject>& JsonObject, const FString& Key, const TArray<FString>& Value)
  {
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    for (const auto& Str : Value)
    {
      JsonArray.Add(MakeShared<FJsonValueString>(Str));
    }
    JsonObject->SetArrayField(Key, JsonArray);
  }

  static void AddFieldByType(TSharedPtr<FJsonObject>& JsonObject, const FString& Key, const TArray<const char*>& Value)
  {
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    for (const auto& Str : Value)
    {
      JsonArray.Add(MakeShared<FJsonValueString>(FString(UTF8_TO_TCHAR(Str))));
    }
    JsonObject->SetArrayField(Key, JsonArray);
  }

  static void AddFieldByType(TSharedPtr<FJsonObject>& JsonObject, const FString& Key, const TArray<double>& Value)
  {
    TArray<TSharedPtr<FJsonValue>> JsonArray;
    for (const auto& Num : Value)
    {
      JsonArray.Add(MakeShared<FJsonValueNumber>(Num));
    }
    JsonObject->SetArrayField(Key, JsonArray);
  }

  static void AddFieldByType(TSharedPtr<FJsonObject>& JsonObject, const FString& Key, const TSharedPtr<FJsonObject>& Value)
  {
    JsonObject->SetObjectField(Key, Value);
  }
};