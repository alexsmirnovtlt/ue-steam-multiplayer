using UnrealBuildTool;

public class OnlineSubsystemHelpersClient : ModuleRules
{
	public OnlineSubsystemHelpersClient(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",

			"OnlineSubsystem",

			"OnlineSubsystemHelpersCommon"
		});
	}
}