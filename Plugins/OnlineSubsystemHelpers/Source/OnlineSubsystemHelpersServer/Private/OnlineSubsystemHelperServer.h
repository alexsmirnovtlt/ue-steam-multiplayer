#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "ThirdParty/Steamworks/Steamv161/sdk/public/steam/steam_api.h"

#include "OnlineSubsystemHelperServer.generated.h"

namespace EOnJoinSessionCompleteResult
{
    enum Type : int;
}

UCLASS()
class UOnlineSubsystemHelperServer : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    void CreateSession();
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

private:
    STEAM_GAMESERVER_CALLBACK(UOnlineSubsystemHelperServer, OnSteamServersConnected, SteamServersConnected_t);
    //TODO can also define SteamServerConnectFailure_t SteamServersDisconnected_t

    FDelegateHandle CloseSessionDelegateHandle;
};