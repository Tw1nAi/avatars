// Copyright Juice sp. z o. o., All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class AvatarsTarget : TargetRules
{
    public AvatarsTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;

        ExtraModuleNames.AddRange(new string[] { "Avatars" });
    }
}
