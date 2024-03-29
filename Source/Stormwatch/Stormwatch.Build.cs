// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

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
		
		// PrivateDependencyModuleNames.AddRange(new string[] { "RiderLink" }); //Comment this out if you're not using Rider.
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
		
		PublicDefinitions.Add("WITH_REACOUSTIC=1");

		PublicIncludePaths.AddRange(
			new string[] 
			{
				"$(ProjectDir)/Source/Stormwatch/Actors/Public",
				"$(ProjectDir)/Source/Stormwatch/Audio/Public",
				"$(ProjectDir)/Source/Stormwatch/Components/Public",
				"$(ProjectDir)/Source/Stormwatch/Core/Public",
				"$(ProjectDir)/Source/Stormwatch/Interfaces/Public",
				"$(ProjectDir)/Source/Stormwatch/Libraries/Public",
				"$(ProjectDir)/Source/Stormwatch/Nightstalker/Public",
				"$(ProjectDir)/Source/Stormwatch/PlayerCharacter/Public",
				"$(ProjectDir)/Source/Stormwatch/RoomSystem/Public"
			}
		);
	}
}
