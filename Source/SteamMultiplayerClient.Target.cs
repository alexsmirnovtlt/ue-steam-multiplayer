using UnrealBuildTool;
using System.Collections.Generic;

public class SteamMultiplayerClientTarget : TargetRules
{
	public SteamMultiplayerClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		ExtraModuleNames.AddRange( new string[] { "SteamMultiplayer" } );
	}
}
