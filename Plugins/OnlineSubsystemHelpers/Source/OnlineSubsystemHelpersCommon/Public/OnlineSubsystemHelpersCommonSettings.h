#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "OnlineSubsystemHelpersCommonSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, DisplayName = "Online Subsystem Helper")
class ONLINESUBSYSTEMHELPERSCOMMON_API UOnlineSubsystemHelpersCommonSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UOnlineSubsystemHelpersCommonSettings(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "General Settings")
	bool bClientCanHost = true;
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "General Settings")
	FName DefaultSessionName = FName(TEXT("SESSION"));
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "General Settings", meta = (Tooltip = "Some random string to filter out servers for 480 dev Steam AppID"))
	FString LobbyUniqueString = FString("iEoipq3BVA");
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "General Settings")
	FString DefaultLobbyName = FString("Test Lobby");
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Session Creation Settings")
	int32 NumPublicConnections = 4;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Session Search Settings")
	int32 MaxSearchResults = 50;
};
