// Copyright 2023 Barrelhouse

using UnrealBuildTool; 

public class BarrelhouseUtilities: ModuleRules
{
	public BarrelhouseUtilities(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"}); 
	}
}
