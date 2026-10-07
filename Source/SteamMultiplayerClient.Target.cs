using UnrealBuildTool;
using UnrealBuildBase;
using System.Collections.Generic;

public class SteamMultiplayerClientTarget : TargetRules
{
	public SteamMultiplayerClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		ExtraModuleNames.AddRange( new string[] { "SteamMultiplayer" } );

		if (!Unreal.IsEngineInstalled())
		{
			BuildEnvironment = TargetBuildEnvironment.Unique;

			GlobalDefinitions.Add("UE_PROJECT_STEAMGAMEDIR=\"spacewar\"");
			GlobalDefinitions.Add("UE_PROJECT_STEAMSHIPPINGID=480");

			CustomConfig = "Client"; // for overrides at Config/Client/
		}
	}
}
