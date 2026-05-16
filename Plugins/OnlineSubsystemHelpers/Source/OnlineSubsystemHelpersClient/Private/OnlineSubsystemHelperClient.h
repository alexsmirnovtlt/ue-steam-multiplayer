#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "OnlineSubsystemHelperClient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnlineSubsystemHelperDelegateNoParams);

namespace EOnJoinSessionCompleteResult
{
    enum Type : int;
}

UCLASS()
class UOnlineSubsystemHelperClient : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
    UFUNCTION(BlueprintCallable)
    void Login();
    void PostLogin();
    UPROPERTY(BlueprintReadOnly)
    bool bLoggedIn = false;
    void OnLoggedIn(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
    UPROPERTY(BlueprintAssignable)
    FOnlineSubsystemHelperDelegateNoParams OnLoginCompleted;

    UFUNCTION(BlueprintCallable)
    void FindSessions();
    TSharedPtr<class FOnlineSessionSearch> LobbySearchSettings;
    void OnFindSessionsCompleted(bool bWasSuccessful);
    UPROPERTY(BlueprintAssignable)
    FOnlineSubsystemHelperDelegateNoParams OnSessionsFound;
    UFUNCTION(BlueprintCallable)
    TMap<int32, FString> GetSessionSearchResults();

    UFUNCTION(BlueprintCallable)
    void Join(int32 SearchResultsIndex, FString AdditionalJoinOptions);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    UPROPERTY(BlueprintAssignable)
    FOnlineSubsystemHelperDelegateNoParams OnSessionCreated;

    UFUNCTION(BlueprintCallable)
    void Host(FString LobbyName);
    
    UFUNCTION(BlueprintCallable)
    bool HaveOpenedSession() const;
    UFUNCTION(BlueprintCallable)
    void CloseOpenedSession();

private:
    FDelegateHandle CurrentDelegateHandle;
    FString AdditionalOptions = "";
};