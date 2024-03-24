// Copyright (c) 2022-present Barrelhouse. All rights reserved.

using UnrealBuildTool;

public class Stormwatch : ModuleRules
{
	public Stormwatch(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"PhysicsCore", 
			"Niagara", 
			"Reacoustic", 
			"Synthesis", 
			"Chaos",
			"AIModule"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"MetasoundEngine",
			"AnimGraphRuntime"
		});
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
		
		PublicIncludePaths.AddRange(
			new string[] 
			{
				"$(ProjectDir)/Source/Stormwatch/Actors/Nightstalker/Public",
				"$(ProjectDir)/Source/Stormwatch/Actors/PlayerCharacter/Public",
				"$(ProjectDir)/Source/Stormwatch/Actors/PlayerController/Public",
				"$(ProjectDir)/Source/Stormwatch/Actors/Public/",
				"$(ProjectDir)/Source/Stormwatch/Audio/Public",
				"$(ProjectDir)/Source/Stormwatch/Components/Public",
				"$(ProjectDir)/Source/Stormwatch/Core/Public",
				"$(ProjectDir)/Source/Stormwatch/Interfaces/Public",
				"$(ProjectDir)/Source/Stormwatch/Libraries/Public",
				"$(ProjectDir)/Source/Stormwatch/Types/Public"
			}
		);
	}
}
