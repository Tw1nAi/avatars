// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SimundiAi : ModuleRules
{
    public SimundiAi(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "AiIntellect", "HTTP", "Json", "JsonUtilities", "RestApi", "AiIdentity", "AiIntellect" });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
