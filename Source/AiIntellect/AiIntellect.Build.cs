// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class AiIntellect : ModuleRules
{
    public AiIntellect(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "AiIdentity" });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
