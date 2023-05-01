// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

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
