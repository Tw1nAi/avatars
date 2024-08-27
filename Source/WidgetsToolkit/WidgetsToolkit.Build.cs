// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class WidgetsToolkit : ModuleRules
{
  public WidgetsToolkit(ReadOnlyTargetRules Target) : base(Target)
  {
    PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

    PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UMG", "SlateCore" });

    PrivateDependencyModuleNames.AddRange(new string[] { });
  }
}
