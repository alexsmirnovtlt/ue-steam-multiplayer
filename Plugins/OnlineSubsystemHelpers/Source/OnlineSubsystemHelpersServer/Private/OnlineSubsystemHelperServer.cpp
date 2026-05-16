#include "OnlineSubsystemHelperServer.h"

#include "OnlineSubsystemHelpersCommonSettings.h"
#include "OnlineSubsystemHelpersServer/OnlineSubsystemHelpersServer.h"

#include "OnlineSubsystemNames.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineIdentityInterface.h"

#include "ThirdParty/Steamworks/Steamv161/sdk/public/steam/steam_gameserver.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OnlineSubsystemHelperServer)

bool UOnlineSubsystemHelperServer::ShouldCreateSubsystem(UObject* Outer) const
{
	return Outer->GetWorld()->IsNetMode(NM_DedicatedServer);
}

void UOnlineSubsystemHelperServer::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (IOnlineSubsystem::Get()->GetSubsystemName() == NULL_SUBSYSTEM)
	{
		auto Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
		Identity->AutoLogin(0);
		CreateSession();
	}
	else if (IOnlineSubsystem::Get()->GetSubsystemName() == STEAM_SUBSYSTEM)
	{
		FString SteamLoginToken;
		if (FParse::Value(FCommandLine::Get(), TEXT("steam_token="), SteamLoginToken))
			SteamGameServer()->LogOn(TCHAR_TO_ANSI(*SteamLoginToken)); 
		else
			SteamGameServer()->LogOnAnonymous();
	}
	else
	{
		UE_LOG(OnlineSubsystemHelpersServerLog, Error,
			TEXT("Cannot handle subsystem %s"), *IOnlineSubsystem::Get()->GetSubsystemName().ToString());
	}
}

void UOnlineSubsystemHelperServer::CreateSession()
{
	const auto Settings = GetDefault<UOnlineSubsystemHelpersCommonSettings>();

	FOnlineSessionSettings SessionSettings = FOnlineSessionSettings();
	SessionSettings.NumPublicConnections = Settings->NumPublicConnections;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = true;
#if WITH_EDITOR
	SessionSettings.bIsLANMatch = true;
#endif

	FString CustomLobbyName; // lobby name as predefined mapname value
	if (!FParse::Value(FCommandLine::Get(), TEXT("lobbyname="), CustomLobbyName))
		CustomLobbyName = Settings->DefaultLobbyName;

	SessionSettings.Set(SETTING_MAPNAME, CustomLobbyName, EOnlineDataAdvertisementType::Type::ViaOnlineService);
	SessionSettings.Set(SEARCH_KEYWORDS, Settings->LobbyUniqueString, EOnlineDataAdvertisementType::Type::ViaOnlineService); // 480 appid only

	UE_LOG(OnlineSubsystemHelpersServerLog, Warning,
		TEXT("Creating new session with %d additional keys"), SessionSettings.Settings.Num());

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	CreateSessionCompleteDelegate.BindUObject(this, &UOnlineSubsystemHelperServer::OnCreateSessionComplete);
	IOnlineSubsystem::Get()->GetSessionInterface()->AddOnCreateSessionCompleteDelegate_Handle(
		MoveTemp(CreateSessionCompleteDelegate));

	IOnlineSubsystem::Get()->GetSessionInterface()->CreateSession(0, Settings->DefaultSessionName, SessionSettings);
}

void UOnlineSubsystemHelperServer::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(OnlineSubsystemHelpersServerLog, Warning, 
		TEXT("OnCreateSessionComplete success: %s"), bWasSuccessful ? *FString("true") : *FString("false"));
}

void UOnlineSubsystemHelperServer::OnSteamServersConnected(SteamServersConnected_t* CallbackData)
{
	if(this->HasAnyFlags(RF_ClassDefaultObject))
	{
		UE_LOG(OnlineSubsystemHelpersServerLog, Warning, TEXT("CDO got callback from steam ?!"));
		return;
	}

	UE_LOG(OnlineSubsystemHelpersServerLog, Warning, TEXT("Log in with Steam successful"));

	// Close currently open session first if needed // TODO needs to be put somewhere more appropriate
	/*
	const auto Settings = GetDefault<UOnlineSubsystemHelpersCommonSettings>();
	if (IOnlineSubsystem::Get()->GetSessionInterface()->GetNamedSession(Settings->DefaultSessionName) != nullptr)
	{
		UE_LOG(OnlineSubsystemHelpersServerLog, Warning,
			TEXT("Destroying session %s"), *Settings->DefaultSessionName.ToString());

		FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
		DestroySessionCompleteDelegate.BindLambda([this](FName SessionName, bool bWasSuccessful)
		{
			IOnlineSubsystem::Get()->GetSessionInterface()->ClearOnDestroySessionRequestedDelegate_Handle(CloseSessionDelegateHandle);
			CreateSession();
		});
		CloseSessionDelegateHandle = IOnlineSubsystem::Get()->GetSessionInterface()->AddOnDestroySessionCompleteDelegate_Handle(
			MoveTemp(DestroySessionCompleteDelegate));
	}
	else*/ CreateSession();
}