// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "OnlineMPSessionsSubsystem.generated.h"


// Delcaring our own custom delegates for the Menu class to bind callbacks to
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMPOnCreateSessionComplete, bool, bWasSuccessful);

//这里使用多播，而不是动态多播(因为里面的参数不是U开头的，不支持动态多播）
DECLARE_MULTICAST_DELEGATE_TwoParams(FMPOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResult, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMPOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMPOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMPOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class ONLINEMPSESSIONS_API UOnlineMPSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UOnlineMPSessionsSubsystem();

	// To handle session functionality. The Menu class will call these.

	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();

	// Our own custom delegates for Menu class to bind callbacks 
	FMPOnCreateSessionComplete  MPOnCreateSessionComplete;
	FMPOnFindSessionsComplete	MPOnFindSessionsComplete;
	FMPOnJoinSessionComplete	MPOnJoinSessionComplete;
	FMPOnDestroySessionComplete MPOnDestroySessionComplete;
	FMPOnStartSessionComplete	MPOnStartSessionComplete;
protected:

	// Internal callbacks for the delegates we'll add to the Online Session Interface delegate list.
	// These don't need to be called outside this class.
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	// To add to the Online Session Interface delegate list.
	// We'll bind our OnlineMPSessionsSubsystem internal callback to these.
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;

	// 委托句柄（Delegate Handle）可以动态的管理委托，在不需要委托时安全地解绑委托。
	FDelegateHandle CreateSessionCompleteHandle;
	FDelegateHandle FindSessionsCompleteHandle;
	FDelegateHandle JoinSessionCompleteHandle;
	FDelegateHandle DestroySessionCompleteHandle;
	FDelegateHandle StartSessionCompleteHandle;

	bool bCreateSessionOnDestroy{ false };
	int32 LastNumPublicConnections;
	FString LastMatchType;
};
