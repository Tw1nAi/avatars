// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RestApi : ModuleRules
{
    public RestApi(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "HTTP", "Json", "JsonUtilities" });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
