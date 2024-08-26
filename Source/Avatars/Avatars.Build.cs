// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Avatars : ModuleRules
{
  public Avatars(ReadOnlyTargetRules Target) : base(Target)
  {
    PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

    // custom modules
    PublicDependencyModuleNames.AddRange(new string[] {
        "CustomGameUserSettings",
        "WidgetsToolkit",
        "TextHelpers",
        "AiIntellect",
        "RestApi",
        "WebsocketAudioStream",
        "JsonHelpers",
        "SimundiAi",
        "AiIdentity"
    });

    // engine modules
    PublicDependencyModuleNames.AddRange(new string[] {
        "Core",
        "CoreUObject",
        "Engine",
        "InputCore",
        "HTTP",
        "WebSockets",
        "Json",
        "JsonUtilities",
        "UMG",
        "AssetRegistry",
        "SlateCore"
    });

    PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem" });

    if (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.DebugGame)
    {
      PublicDefinitions.Add("UE_DEBUG_GC=1");
    }

    PublicIncludePaths.AddRange(
        new string[] {
                "Avatars/Public/Utils/Public"
        }
    );

    PrivateIncludePaths.AddRange(
        new string[] {
                "Avatars/Private/Utils/Private"
        }
    );
  }
}
