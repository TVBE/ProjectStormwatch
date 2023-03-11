// Copyright 2023 Barrelhouse

using UnrealBuildTool;

public class Frostbite : ModuleRules
{
	public Frostbite(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "PhysicsCore", "Niagara" });

		PrivateDependencyModuleNames.AddRange(new string[] { "RiderLink", "MetasoundEngine", "AnimGraphRuntime" });
		
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/Core/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/Environment/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/RoomSystem/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/Audio/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/PlayerCharacter/Public");
		PublicIncludePaths.Add("$(ProjectDir)/Source/Frostbite/Nightstalker/Public");
	}
}
