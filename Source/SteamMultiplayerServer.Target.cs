using UnrealBuildTool;
using System.Collections.Generic;

public class SteamMultiplayerServerTarget : TargetRules
{
	public SteamMultiplayerServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		ExtraModuleNames.AddRange( new string[] { "SteamMultiplayer" } );

		GlobalDefinitions.Add("UE_PROJECT_STEAMPRODUCTNAME=\"Spacewar\"");
		GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDIR=\"Spacewar\"");
		GlobalDefinitions.Add("UE_PROJECT_STEAMSHIPPINGID=480");
		GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDESC=\"Spacewar\"");
	}
}
