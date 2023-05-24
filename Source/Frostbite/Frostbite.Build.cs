// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

using UnrealBuildTool;

public class Frostbite : ModuleRules
{
	public Frostbite(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "PhysicsCore", "Niagara", "Reacoustic", "Synthesis", "Chaos" });

		PrivateDependencyModuleNames.AddRange(new string[] { "RiderLink", "MetasoundEngine", "AnimGraphRuntime"});
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}
		
		PublicDefinitions.Add("WITH_REACOUSTIC=1");

		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/Actors/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/Core/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/Interfaces/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/Libraries/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/RoomSystem/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/Audio/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/PlayerCharacter/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/Nightstalker/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/ActorComponents/Public");
	}
}
