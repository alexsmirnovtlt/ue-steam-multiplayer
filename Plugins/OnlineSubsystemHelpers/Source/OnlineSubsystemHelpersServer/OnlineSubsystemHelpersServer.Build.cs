using UnrealBuildTool;

public class OnlineSubsystemHelpersServer : ModuleRules
{
	public OnlineSubsystemHelpersServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",

			"OnlineSubsystem",

			"OnlineSubsystemHelpersCommon"
		});

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		AddEngineThirdPartyPrivateStaticDependencies(Target, "Steamworks");
	}
}