// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/GameInstance.h>
#include <Interfaces/OnlineSessionInterface.h>

#include "R4GameInstance.generated.h"

// singleton 선언 매크로
#define DECLARE_SINGLETON( Type, Name )			\
private:										\
	TUniquePtr<Type> Name;						\
public:											\
	const TUniquePtr<Type>& Get##Name() const	\
	{ return Name; }

class FOnlineSessionSearch;
class FDataTableManager;
class FObjectPool;

/**
 * GameInstance
 */
UCLASS()
class RAID4_API UR4GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UR4GameInstance();
	
	// 게임 시작시 호출
	virtual void Init() override;

	// 게임 종료시 호출
	virtual void Shutdown() override;

	// Session Find 시 Delegate
	DECLARE_MULTICAST_DELEGATE_OneParam( FOnFindSessionCompleteDelegate, const TArray<FOnlineSessionSearchResult>& );
	FOnFindSessionCompleteDelegate OnFindSessionCompleteDelegate;
	
public:
	// Session 생성
	UFUNCTION( BlueprintCallable, Category = "Multiplayer" )
	void CreateGameSession( bool InIsLanMatch );

	// Session Find
	UFUNCTION( BlueprintCallable, Category = "Multiplayer" )
	void FindGameSession( bool InIsLanMatch, int32 InMaxSearchNum );

	// Session Join
	UFUNCTION( BlueprintCallable, Category = "Multiplayer" )
	void JoinGameSession( int32 InResultIndex );
	
	// Main Game으로 이동
	void TravelToMainGame() const;

	// Lobby로 이동
	void TravelToLobby() const;
private:
	// On Session Create
	void _OnCreateGameSessionComplete( FName InSessionName, bool InIsSuccessful );

	// Find Session Complete
	void _OnFindGameSessionComplete( bool InIsSuccessful ) const;

	// Join Session Complete
	void _OnJoinGameSessionComplete( FName InSessionName, EOnJoinSessionCompleteResult::Type InType );

	// Destroy Session.
	void _DestroyGameSession() const;
	
	//// singleton ////
	// Singleton 처럼 1개의 인스턴스만 쓰기 위한 Object들 선언
	DECLARE_SINGLETON( FDataTableManager, DataTableManager )
	DECLARE_SINGLETON( FObjectPool, ObjectPool )

	// singleton들을 등록
	void _CreateSingletons();

	// singleton들을 정리
	void _ClearSingletons();

	//////////////////
private:
	// Lobby (Main Menu) Level
	UPROPERTY( EditDefaultsOnly, Category = "Level" )
	TSoftObjectPtr<UWorld> LobbyLevel;
	
	// Session 생성 시 Transition할 CharacterPickLevel
	UPROPERTY( EditDefaultsOnly, Category = "Level" )
	TSoftObjectPtr<UWorld> CharacterPickLevel;

	// Main Game Level
	UPROPERTY( EditDefaultsOnly, Category = "Level" )
	TSoftObjectPtr<UWorld> MainGameLevel;

	// Session 생성 시 MaxPlayerNum
	UPROPERTY( EditDefaultsOnly, Category = "Level" )
	int32 MaxPlayerNum;
	
	// Session Search 결과
	TSharedPtr<FOnlineSessionSearch> CachedSessionSearch;
};

// Singleton 생성 매크로. _CreateSingletons() 내부에서 사용
#define CREATE_SINGLETON( Type, Name ) \
	Name = MakeUnique<Type>();		   \
	Name->InitSingleton();

// Singleton Clear 매크로. _ClearSingletons() 내부에서 사용
#define CLEAR_SINGLETON( Name ) \
	Name->ClearSingleton();		\
	Name.Reset();