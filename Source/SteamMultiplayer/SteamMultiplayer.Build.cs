using UnrealBuildTool;

public class SteamMultiplayer : ModuleRules
{
	public SteamMultiplayer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "Engine"});
	}
}
