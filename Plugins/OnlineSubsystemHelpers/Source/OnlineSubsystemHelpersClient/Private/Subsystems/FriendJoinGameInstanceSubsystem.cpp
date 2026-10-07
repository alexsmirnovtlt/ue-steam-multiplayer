#include "FriendJoinGameInstanceSubsystem.h"

#include "OnlineSubsystemHelpersClient/OnlineSubsystemHelpersClient.h"
#include "OnlineSubsystemHelpersCommonSettings.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/GameInstance.h"

bool UFriendJoinGameInstanceSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return WITH_SERVER_CODE && !WITH_EDITOR; // Game target only (which can be both Listen server and Client)
}

void UFriendJoinGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (auto OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if(auto Session = OnlineSubsystem->GetSessionInterface())
		{
			UE_LOG(OnlineSubsystemHelpersClientLog, Log, TEXT("Subscribing on SessionUserInviteAccepted"));
			Session->OnSessionUserInviteAcceptedDelegates.AddUObject(
				this, &UFriendJoinGameInstanceSubsystem::OnSessionUserInviteAccepted);
		}
	}
}

void UFriendJoinGameInstanceSubsystem::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId,
	FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	UE_LOG(OnlineSubsystemHelpersClientLog, Log,
		TEXT("OnSessionUserInviteAccepted received with bWasSuccessful: %d, InviteResult.IsValid(): %d"),
		bWasSuccessful, InviteResult.IsValid());

	if (!bWasSuccessful || !InviteResult.IsValid()) return;

	auto Session = IOnlineSubsystem::Get()->GetSessionInterface();

	if(Session->GetNamedSession(NAME_GameSession) != nullptr)
	{
		UE_LOG(OnlineSubsystemHelpersClientLog, Error, TEXT("User already in a session, cannot join by invite"));
		return;
	}

	JoinSessionCompleteHandle = Session->OnJoinSessionCompleteDelegates.AddUObject(
		this, &UFriendJoinGameInstanceSubsystem::OnJoinSessionComplete);

	Session->JoinSession(ControllerId, NAME_GameSession, InviteResult);
}

void UFriendJoinGameInstanceSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	auto Session = IOnlineSubsystem::Get()->GetSessionInterface();
	Session->OnJoinSessionCompleteDelegates.Remove(JoinSessionCompleteHandle);

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(OnlineSubsystemHelpersClientLog, Error, TEXT("Cannot join: %s'"), LexToString(Result));
		return;
	}

	FString ConnectString;
	if(Session->GetResolvedConnectString(SessionName, ConnectString))
	{
		UE_LOG(OnlineSubsystemHelpersClientLog, Log, TEXT("Resolved ConnectString is '%s'"), *ConnectString);
		if(ensure(GetGameInstance()->GetFirstLocalPlayerController()))  // might have no valid world with valid PC
			GetGameInstance()->GetFirstLocalPlayerController()->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
	}
}