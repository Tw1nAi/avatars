// Copyright Juice sp. z o. o., All Rights Reserved.

using UnrealBuildTool;

public class Avatars : ModuleRules
{
    public Avatars(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HTTP", "WebSockets", "Json", "JsonUtilities", "UMG", "AssetRegistry", "SlateCore", "RuntimeAudioImporter", "AudioCaptureCore", "AudioCapture", "AudioMixer" });

        PrivateDependencyModuleNames.AddRange(new string[] { });

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

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
