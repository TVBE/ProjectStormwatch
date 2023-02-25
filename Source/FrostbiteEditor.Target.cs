// Copyright 2023 Barrelhouse

using UnrealBuildTool;
using System.Collections.Generic;

public class FrostbiteEditorTarget : TargetRules
{
	public FrostbiteEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Frostbite" } );
	}
}
