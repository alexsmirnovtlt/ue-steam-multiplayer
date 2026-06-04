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

		GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDIR=\"spacewar\"");
		GlobalDefinitions.Add("UE_PROJECT_STEAMSHIPPINGID=480");
	}
}
