// Copyright Juice sp. z o. o., All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class AvatarsEditorTarget : TargetRules
{
    public AvatarsEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V2;

        ExtraModuleNames.Add("TextHelpers");
        ExtraModuleNames.AddRange(new string[] { "Avatars" });
    }
}
