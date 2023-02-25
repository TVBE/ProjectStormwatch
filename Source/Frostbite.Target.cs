// Copyright 2023 Barrelhouse

using UnrealBuildTool;
using System.Collections.Generic;

public class FrostbiteTarget : TargetRules
{
	public FrostbiteTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Frostbite" } );
	}
}
