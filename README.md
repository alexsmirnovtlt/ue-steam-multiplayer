# ue-steam-multiplayer
Basic multiplayer functionality for Dedicated and Listen Servers with Steam backend

# How to check if its working

## PIE Listen Server (Subsystem NULL)
 - Editor Preferences -> "Run Under One Process": false
 - Net Mode: Standalone
 - Number of Players: 2 or more
 - Standalone instance hosts the game and PIE instance joins (vice versa doesn`t work apparently)
 - Steam: Not open in the background

## PIE Dedicated Server + Clients (Subsystem NULL)
 - Editor Preferences -> "Launch Separate Server": false
 - Editor Preferences -> "Run Under One Process": true/false
 - Net Mode: Standalone
 - Number of Players: 2 or more
 - Steam: Not open in the background

 - Server must be started prior to PIE play. For example one can use the following .bat file which is put next to .uproject:
> start "" "C:/Some/Path/To/Unreal/UE_5.7/Engine/Binaries/Win64/UnrealEditor.exe" "%CD%/SteamMultiplayer.uproject" /OnlineSubsystemHelpers/Maps/GameplayLevel -server -log -nosteam -lobbyname="my custom lobby"

## Dedicated server (Subsystem Steam)
 - Build Targets: Development Server and Development Client

 - Server can be started with the following .bat file (-lobbyname and -steam_token are optional):
> start SteamMultiplayerServer.exe -lobbyname="my custom lobby" -log -steam_token="LETTERS_AND_NUMBERS_TOKEN"

### Notes:
 * Dedicated Server requires steam_appid.txt at SteamMultiplayer/Binaries/Win64/ to be put manually as of UE 5.7
 * Steam token can be created at https://steamcommunity.com/dev/managegameservers

## Listen server (Subsystem Steam)
 - Build Target: Development
 - Two different machines with different Steam accounts are required. One machine hosts and other joins

# Useful links
## Official guides:
 * https://dev.epicgames.com/documentation/unreal-engine/online-subsystem-steam-interface-in-unreal-engine?application_version=5.7
 * https://dev.epicgames.com/documentation/unreal-engine/using-steam-sockets-in-unreal-engine?application_version=5.7
 * https://dev.epicgames.com/documentation/unreal-engine/API/Plugins/OnlineSubsystem/FOnlineSessionSettings?application_version=5.7

## Other links
 * https://github.com/Luomu/ue-dediserversample
 * https://forums.unrealengine.com/t/unexpected-gspolicyresponse-callback-ue5-dedicated-steam-server/2663077/15
 * https://kb.heathen.group/steam/configuration/unreal-configuration

# Troobleshooting
 * Dedicated server shown on Steam -> View -> Game Servers is on 'LAN' tab instead of 'Internet' (UE5.5) or server creation fails with "LogOnline: Warning: STEAM: Unexpected GSPolicyResponse callback" (UE5.7)
     * getting public static IP from my Internet Provider actually helped to resolve both issues. See CGNAT(Carrier-Grade NAT)
