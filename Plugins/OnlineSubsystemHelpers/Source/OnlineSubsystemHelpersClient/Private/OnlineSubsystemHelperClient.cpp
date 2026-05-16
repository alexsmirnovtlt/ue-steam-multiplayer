#include "OnlineSubsystemHelperClient.h"

#include "OnlineSubsystemHelpersClient/OnlineSubsystemHelpersClient.h"
#include "OnlineSubsystemHelpersCommonSettings.h"

#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineIdentityInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OnlineSubsystemHelperClient)

bool UOnlineSubsystemHelperClient::ShouldCreateSubsystem(UObject* Outer) const
{
	return Outer->GetWorld()->IsNetMode(NM_Standalone);
}

void UOnlineSubsystemHelperClient::Login()
{
	UE_LOG(OnlineSubsystemHelpersClientLog, Warning, TEXT("Online subsystem is %s"),
		*IOnlineSubsystem::Get()->GetSubsystemName().ToString());

	auto IdentityInterface = IOnlineSubsystem::Get()->GetIdentityInterface();
	if (IdentityInterface->GetLoginStatus(0) != ELoginStatus::LoggedIn)
	{
		FOnLoginCompleteDelegate LoginCompleteDelegate;
		LoginCompleteDelegate.BindUObject(this, &UOnlineSubsystemHelperClient::OnLoggedIn);
		CurrentDelegateHandle = IdentityInterface->AddOnLoginCompleteDelegate_Handle(0, MoveTemp(LoginCompleteDelegate));
	}
	else
		PostLogin();
}

void UOnlineSubsystemHelperClient::OnLoggedIn(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	IOnlineSubsystem::Get()->GetIdentityInterface()->ClearOnLoginCompleteDelegate_Handle(0, CurrentDelegateHandle);

	if (!bWasSuccessful)
	{
		UE_LOG(OnlineSubsystemHelpersClientLog, Error, TEXT("Login error: %s"), *Error);
	}
}

void UOnlineSubsystemHelperClient::PostLogin()
{
	bLoggedIn = true;

	auto Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
	UE_LOG(OnlineSubsystemHelpersClientLog, Warning, TEXT("Login successful. PlatformUserId: %d. Nickname: %s"),
		Identity->GetPlatformUserIdFromLocalUserNum(0).GetInternalId(),
		*Identity->GetPlayerNickname(0));

	OnLoginCompleted.Broadcast();
}

void UOnlineSubsystemHelperClient::FindSessions()
{
	if (!LobbySearchSettings.IsValid())
	{
		const auto Settings = GetDefault<UOnlineSubsystemHelpersCommonSettings>();
		LobbySearchSettings = MakeShared<FOnlineSessionSearch>();

		LobbySearchSettings.Get()->MaxSearchResults = Settings->MaxSearchResults;
#if WITH_EDITOR
		LobbySearchSettings.Get()->bIsLanQuery = true;
#endif

#if WITH_CLIENT_CODE
		LobbySearchSettings.Get()->QuerySettings.Set(SEARCH_DEDICATED_ONLY, true, EOnlineComparisonOp::Equals);
#endif
		LobbySearchSettings.Get()->QuerySettings.Set(SEARCH_KEYWORDS, Settings->LobbyUniqueString, EOnlineComparisonOp::Equals); // 480 appid only
	}

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FindSessionsCompleteDelegate.BindUObject(this, &UOnlineSubsystemHelperClient::OnFindSessionsCompleted);

	CurrentDelegateHandle = IOnlineSubsystem::Get()->GetSessionInterface()->
		AddOnFindSessionsCompleteDelegate_Handle(MoveTemp(FindSessionsCompleteDelegate));
	IOnlineSubsystem::Get()->GetSessionInterface()->FindSessions(0, LobbySearchSettings.ToSharedRef());
}

void UOnlineSubsystemHelperClient::OnFindSessionsCompleted(bool bWasSuccessful)
{
	IOnlineSubsystem::Get()->GetSessionInterface()->ClearOnFindSessionsCompleteDelegate_Handle(CurrentDelegateHandle);

	if (!ensure(bWasSuccessful)) return;
	UE_LOG(OnlineSubsystemHelpersClientLog, Warning, TEXT("Sessions found: %d"), LobbySearchSettings.Get()->SearchResults.Num());
	OnSessionsFound.Broadcast();
}

TMap<int32, FString> UOnlineSubsystemHelperClient::GetSessionSearchResults()
{
	TMap<int32, FString> Results;
	for (uint8 i = 0; i < LobbySearchSettings.Get()->SearchResults.Num(); i++)
	{
		FString LobbyName;
		LobbySearchSettings.Get()->SearchResults[i].Session.SessionSettings.Get(SETTING_MAPNAME, LobbyName);	
		if (!LobbyName.IsEmpty()) Results.Add(i, LobbyName);
	}
	return Results;
}

void UOnlineSubsystemHelperClient::Join(int32 SearchResultsIndex, FString AdditionalJoinOptions)
{
	if (!ensure(LobbySearchSettings.Get()->SearchResults.IsValidIndex(SearchResultsIndex))) return;

	AdditionalOptions = AdditionalJoinOptions;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	JoinSessionCompleteDelegate.BindUObject(this, &UOnlineSubsystemHelperClient::OnJoinSessionComplete);

	CurrentDelegateHandle = IOnlineSubsystem::Get()->GetSessionInterface()->
		AddOnJoinSessionCompleteDelegate_Handle(MoveTemp(JoinSessionCompleteDelegate));
	IOnlineSubsystem::Get()->GetSessionInterface()->JoinSession(
		0,
		GetDefault<UOnlineSubsystemHelpersCommonSettings>()->DefaultSessionName,
		LobbySearchSettings.Get()->SearchResults[SearchResultsIndex]);
}

void UOnlineSubsystemHelperClient::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem::Get()->GetSessionInterface()->ClearOnJoinSessionCompleteDelegate_Handle(CurrentDelegateHandle);

	FString ConnectionInfo;
	IOnlineSubsystem::Get()->GetSessionInterface()->GetResolvedConnectString(
		GetDefault<UOnlineSubsystemHelpersCommonSettings>()->DefaultSessionName, ConnectionInfo);

	UE_LOG(OnlineSubsystemHelpersClientLog, Warning, TEXT("Join session`s connection info str: '%s'"), *ConnectionInfo);
	UE_LOG(OnlineSubsystemHelpersClientLog, Warning, TEXT("Join session`s additional options str: '%s'"), *AdditionalOptions);

	GetWorld()->GetFirstPlayerController()->ClientTravel(ConnectionInfo + AdditionalOptions, ETravelType::TRAVEL_Absolute);
}

void UOnlineSubsystemHelperClient::Host(FString LobbyName)
{
	const auto Settings = GetDefault<UOnlineSubsystemHelpersCommonSettings>();

	FOnlineSessionSettings SessionSettings = FOnlineSessionSettings();
	SessionSettings.NumPublicConnections = Settings->NumPublicConnections;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
#if WITH_EDITOR
	SessionSettings.bIsLANMatch = true;
#endif

	SessionSettings.Set(SETTING_MAPNAME, LobbyName, EOnlineDataAdvertisementType::Type::ViaOnlineService);
	SessionSettings.Set(SEARCH_KEYWORDS, Settings->LobbyUniqueString, EOnlineDataAdvertisementType::Type::ViaOnlineService); // 480 appid only

	UE_LOG(OnlineSubsystemHelpersClientLog, Warning, TEXT("Creating new session with %d additional keys"),
		SessionSettings.Settings.Num());

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	CreateSessionCompleteDelegate.BindUObject(this, &UOnlineSubsystemHelperClient::OnCreateSessionComplete);
	CurrentDelegateHandle = IOnlineSubsystem::Get()->GetSessionInterface()->AddOnCreateSessionCompleteDelegate_Handle(
		MoveTemp(CreateSessionCompleteDelegate));

	IOnlineSubsystem::Get()->GetSessionInterface()->CreateSession(0, Settings->DefaultSessionName, SessionSettings);
}

void UOnlineSubsystemHelperClient::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem::Get()->GetSessionInterface()->ClearOnCreateSessionCompleteDelegate_Handle(CurrentDelegateHandle);

	UE_LOG(OnlineSubsystemHelpersClientLog, Warning, TEXT("OnCreateSessionComplete success: %s"),
		bWasSuccessful ? *FString("true") : *FString("false"));

	if (bWasSuccessful)
		OnSessionCreated.Broadcast();
}

bool UOnlineSubsystemHelperClient::HaveOpenedSession() const
{
	const auto Settings = GetDefault<UOnlineSubsystemHelpersCommonSettings>();
	return IOnlineSubsystem::Get()->GetSessionInterface()->GetNamedSession(Settings->DefaultSessionName) != nullptr;
}

void UOnlineSubsystemHelperClient::CloseOpenedSession()
{
	UE_LOG(OnlineSubsystemHelpersClientLog, Warning, TEXT("Destroying previous session"));

	IOnlineSubsystem::Get()->GetSessionInterface()->DestroySession(
		GetDefault<UOnlineSubsystemHelpersCommonSettings>()->DefaultSessionName);
}