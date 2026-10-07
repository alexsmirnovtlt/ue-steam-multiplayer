#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#ifdef UE_PROJECT_STEAMPRODUCTNAME
#include "steam/steam_api.h"
#endif
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
#ifdef UE_PROJECT_STEAMPRODUCTNAME
    STEAM_GAMESERVER_CALLBACK(UOnlineSubsystemHelperServer, OnSteamServersConnected, SteamServersConnected_t);
    //TODO can also define SteamServerConnectFailure_t SteamServersDisconnected_t
#endif

    FDelegateHandle CloseSessionDelegateHandle;
};