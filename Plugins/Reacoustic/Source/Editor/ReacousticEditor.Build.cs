// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
// Written by Nino Saglia.

using UnrealBuildTool;

public class ReacousticEditor : ModuleRules
{
	public ReacousticEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"AssetTools",
				"Reacoustic", 
				"Blutility", 
				"CoreUObject", 
				"Engine", 
				"InputCore", 
				"ToolMenus",
				"ContentBrowser",
				"UnrealEd", 
				"EditorStyle",
				"EditorFramework",
				"LevelEditor",
				"Core", "CoreUObject", "Engine", "InputCore",
				
				
				
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Settings",
				"InputCore", 
				"PhysicsCore",
				"AudioSynesthesia",
				"UnrealEd",
				"PropertyEditor", "AITestSuite", "WorkspaceMenuStructure","ContentBrowser"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		

		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
