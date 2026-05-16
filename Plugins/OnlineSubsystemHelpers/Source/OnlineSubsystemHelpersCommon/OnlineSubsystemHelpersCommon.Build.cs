using UnrealBuildTool;

public class OnlineSubsystemHelpersCommon : ModuleRules
{
	public OnlineSubsystemHelpersCommon(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",

			"DeveloperSettings"
		});
	}
}