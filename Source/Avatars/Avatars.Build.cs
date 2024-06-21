// Copyright Juice sp. z o. o., All Rights Reserved.

using UnrealBuildTool;

public class Avatars : ModuleRules
{
    public Avatars(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.Add("TextHelpers");
        PublicDependencyModuleNames.Add("AiIntellect");
        PublicDependencyModuleNames.Add("RestApi");
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HTTP", "WebSockets", "Json", "JsonUtilities", "UMG", "AssetRegistry", "SlateCore", "RuntimeAudioImporter", "AudioCaptureCore", "AudioCapture", "AudioMixer" });

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
