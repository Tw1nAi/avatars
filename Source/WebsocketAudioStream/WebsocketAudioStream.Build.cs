// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class WebsocketAudioStream : ModuleRules
{
    public WebsocketAudioStream(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "AudioCapture",
            "WebSockets",
            "AudioMixer",
            "AudioCaptureCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
