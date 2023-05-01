// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

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
