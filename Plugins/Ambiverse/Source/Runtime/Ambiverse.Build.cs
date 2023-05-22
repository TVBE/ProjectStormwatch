// Some copyright should be here...

using UnrealBuildTool;

public class Ambiverse : ModuleRules
{
	public Ambiverse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"$(ProjectDir)/Plugins/Ambiverse/Source/Runtime/Core/Public",
				"$(ProjectDir)/Plugins/Ambiverse/Source/Runtime/Data/Public"
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"MetasoundEngine",
				"AmbiverseEditor"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
