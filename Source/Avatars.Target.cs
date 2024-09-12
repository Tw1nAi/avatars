// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AvatarsTarget : TargetRules
{
  public AvatarsTarget(TargetInfo Target) : base(Target)
  {
    bOverrideBuildEnvironment = true;
    Type = TargetType.Game;
    DefaultBuildSettings = BuildSettingsVersion.V4;
    //IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
    ExtraModuleNames.Add("TextHelpers");
    ExtraModuleNames.Add("AiIntellect");
    ExtraModuleNames.Add("RestApi");
    ExtraModuleNames.Add("WebsocketAudioStream");
    ExtraModuleNames.Add("JsonHelpers");
    ExtraModuleNames.Add("SimundiAi");
    ExtraModuleNames.Add("AiIdentity");
    ExtraModuleNames.Add("WidgetsToolkit");
    ExtraModuleNames.AddRange(new string[] { "Avatars" });
  }
}
