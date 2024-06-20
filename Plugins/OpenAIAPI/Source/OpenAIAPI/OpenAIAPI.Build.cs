// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OpenAIAPI : ModuleRules
{
    public OpenAIAPI(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
                {
                    "Core"
                }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
                {
                    "CoreUObject",
                    "Engine",
                    "Slate",
                    "SlateCore",
                    "Json",
                    "HTTP"
                }
            );
    }
}
