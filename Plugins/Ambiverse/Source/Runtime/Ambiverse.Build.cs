// Some copyright should be here...

using UnrealBuildTool;

public class Ambiverse : ModuleRules
{
	public Ambiverse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"$(ProjectDir)/Plugins/Ambiverse/Source/Runtime/Public",
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"$(ProjectDir)/Plugins/Ambiverse/Source/Runtime/Private",
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
			}
			);
		
		PublicIncludePathModuleNames.AddRange(new string[] { "AmbiverseEditor" });
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
