// Fill out your copyright notice in the Description page of Project Settings.


#include "R4GameInstance.h"

#include "R4LobbyGameMode.h"
#include "../Manager/DataTableManager.h"
#include "../Core/ObjectPool/ObjectPool.h"

#include <OnlineSubsystem.h>
#include <OnlineSessionSettings.h>
#include <GameFramework/PlayerController.h>
#include <Kismet/GameplayStatics.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4GameInstance)

/**
 * 생성자
 */
UR4GameInstance::UR4GameInstance()
{
	MaxPlayerNum = 4;
	CachedSessionSearch = nullptr;
	CachedNetPlayerName = FString();
}

/**
 * 게임 시작시 호출
 */
void UR4GameInstance::Init()
{
	Super::Init();

	_CreateSingletons();

	// Session 함수 관련 Bind
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if ( onlineSubsystem != nullptr )
	{
		IOnlineSessionPtr sessionInterface = onlineSubsystem->GetSessionInterface();
		if ( sessionInterface.IsValid() )
		{
			sessionInterface->OnCreateSessionCompleteDelegates.AddUObject( this, &UR4GameInstance::_OnCreateGameSessionComplete );
			sessionInterface->OnFindSessionsCompleteDelegates.AddUObject( this, &UR4GameInstance::_OnFindGameSessionComplete );
			sessionInterface->OnJoinSessionCompleteDelegates.AddUObject( this, &UR4GameInstance::_OnJoinGameSessionComplete );
		}
	}
}

/**
 * 게임 종료시 호출
 */
void UR4GameInstance::Shutdown()
{
	_ClearSingletons();
	
	Super::Shutdown();
}

/**
 * Session 생성
 * @param InNetPlayerName : Session에서 사용할 Player Name.
 */
void UR4GameInstance::CreateGameSession( bool InIsLanMatch, const FString& InNetPlayerName )
{
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if ( onlineSubsystem != nullptr )
	{
		IOnlineSessionPtr sessionInterface = onlineSubsystem->GetSessionInterface();
		if ( sessionInterface.IsValid() )
		{
			FOnlineSessionSettings settings;
			settings.bIsLANMatch = InIsLanMatch; // LAN 매치?
			settings.NumPublicConnections = MaxPlayerNum; // 최대 플레이어 수
			settings.bShouldAdvertise = true;
			settings.bAllowJoinInProgress = true; // start를 따로 호출.
			settings.bUsesPresence = true;
			settings.bAllowJoinViaPresence = true;
			settings.bUseLobbiesIfAvailable = true;
			
			// Set Host Name
			FOnlineSessionSetting hostName;
			hostName.Data = InNetPlayerName;
			hostName.AdvertisementType = EOnlineDataAdvertisementType::Type::ViaOnlineService;
			settings.Settings.Add( NetGame::G_HostPlayerNameKey, hostName );

			// Caching Name
			CachedNetPlayerName = InNetPlayerName;

			sessionInterface->CreateSession(0, NAME_GameSession, settings );
		}
	}
}

/**
 * Session 검색
 */
void UR4GameInstance::FindGameSession( bool InIsLanMatch, int32 InMaxSearchNum )
{
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if ( onlineSubsystem != nullptr )
	{
		IOnlineSessionPtr sessionInterface = onlineSubsystem->GetSessionInterface();
		if ( sessionInterface.IsValid() )
		{
			CachedSessionSearch = MakeShared<FOnlineSessionSearch>();
			CachedSessionSearch->bIsLanQuery = InIsLanMatch;
			CachedSessionSearch->MaxSearchResults = InMaxSearchNum;
			
			sessionInterface->FindSessions( 0, CachedSessionSearch.ToSharedRef() );
		}
	}
}

/**
 * Join Session
 * @param InResultIndex : CachedSessionSearch의 결과 중 Index
 * @param InNetPlayerName : Session에서 사용할 Player Name.
 */
void UR4GameInstance::JoinGameSession( int32 InResultIndex, const FString& InNetPlayerName )
{
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if ( onlineSubsystem != nullptr )
	{
		IOnlineSessionPtr sessionInterface = onlineSubsystem->GetSessionInterface();
		if ( sessionInterface.IsValid() && CachedSessionSearch->SearchResults.IsValidIndex( InResultIndex ) )
		{
			CachedNetPlayerName = InNetPlayerName;
			sessionInterface->JoinSession( 0, NAME_GameSession, CachedSessionSearch->SearchResults[InResultIndex] );
		}
	}
}

/**
 * Main Game으로 이동 ( Seamless travel )
 */
void UR4GameInstance::TravelToMainGame() const
{
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if ( onlineSubsystem != nullptr )
	{
		IOnlineSessionPtr sessionInterface = onlineSubsystem->GetSessionInterface();
		if ( sessionInterface.IsValid() )
		{
			// travel 가능한지 확인
			UWorld* world = GetWorld();
			if ( !IsValid( world ) )
			{
				LOG_WARN( R4Log, TEXT("GetWorld() is invalid.") );
				return;
			}

			AR4LobbyGameMode* gameMode = world->GetAuthGameMode<AR4LobbyGameMode>();
			if ( !IsValid( gameMode ) )
			{
				LOG_WARN( R4Log, TEXT("LobbyGameMode is invalid.") );
				return;
			}
			
			if ( gameMode->CanTravelMainLevel() )
			{
				// Seamless travel
				gameMode->bUseSeamlessTravel = true;

				// 현재 참여중인 Player가 몇명인지 Options으로 포함하여 호출
				FString mainURL = FString::Printf( TEXT("%s?%s=%d"), *MainGameLevel.GetAssetName(), *NetGame::G_PlayerNumsParamKey, gameMode->GetNumPlayers() );
				world->ServerTravel( mainURL, true );

				gameMode->bUseSeamlessTravel = false;
			}
		}
	}
}

/**
 * Lobby로 이동
 */
void UR4GameInstance::TravelToLobby() const
{
	APlayerController* playerController = GetFirstLocalPlayerController();
	if ( !IsValid( playerController ) )
		return;
	
	_DestroyGameSession();
	playerController->ClientTravel( LobbyLevel.GetAssetName(), TRAVEL_Absolute );
}

/**
 * URL로 Force Travel ( console command )
 */
void UR4GameInstance::ForceTravel( const FString& InUrl, const FString& InNetPlayerName )
{
	APlayerController* playerController = GetFirstLocalPlayerController();
	if ( !IsValid( playerController ) )
		return;
	
	FString travelURL = TEXT("open ") + InUrl;
	if( InNetPlayerName != FString() )
	{
		CachedNetPlayerName = InNetPlayerName;
		_AppendNetPlayerNameParam( travelURL ); 
	}
	
	playerController->ConsoleCommand( travelURL );
}

/**
 * Destroy Session.
 */
void UR4GameInstance::_DestroyGameSession() const
{
	// 서버가 나가면 다같이 바이바이
	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if ( onlineSubsystem != nullptr )
	{
		IOnlineSessionPtr sessionInterface = onlineSubsystem->GetSessionInterface();
		if ( sessionInterface.IsValid() )
		{
			sessionInterface->DestroySession( NAME_GameSession );
		}
	}
}

/**
 * Session 참가 시 사용했던 Name을 반환.
 */
const FString& UR4GameInstance::_GetLocalNetPlayerName() const
{
	return CachedNetPlayerName;
}

/**
 * 주어진 String에 Name parameter를 붙여 반환. ( Network 이동 시 사용 )
 */
void UR4GameInstance::_AppendNetPlayerNameParam( FString& OutString ) const
{
	FString playerNameParam = FString::Printf( TEXT("?%s=%s"), *NetGame::G_PlayerNameParamKey, *_GetLocalNetPlayerName() );
	OutString.Append( playerNameParam );
}

/**
 * On Session Create
 */
void UR4GameInstance::_OnCreateGameSessionComplete( FName InSessionName, bool InIsSuccessful )
{
	if ( !InIsSuccessful )
	{
		LOG_WARN( R4Log, TEXT("Create Session Failed.") );
		_DestroyGameSession();
		return;
	}

	LOG_N( R4Log, TEXT("Create Session Success.") );

    IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
    if ( onlineSubsystem != nullptr )
    {
    	IOnlineSessionPtr sessionInterface = onlineSubsystem->GetSessionInterface();
    	if ( sessionInterface.IsValid() )
    	{
    		if ( !sessionInterface->StartSession( NAME_GameSession ) )
    		{
    			LOG_N( R4Log, TEXT("Start Session Failed.") );
    			return;
    		}
    		
    		LOG_N( R4Log, TEXT("Start Session Success.") );

    		UWorld* world = GetWorld();
    		if ( IsValid( world ) )
    		{
    			LOG_N( R4Log, TEXT("Open Level.") );
    		
    			// NonSeamless travel
    			FString option = TEXT( "listen" );
    			_AppendNetPlayerNameParam( option );
    			UGameplayStatics::OpenLevel( world, FName(CharacterPickLevel.GetAssetName()), true, option );
    		}
    	}
    }
}

/**
 * Find Session Complete
 */
void UR4GameInstance::_OnFindGameSessionComplete( bool InIsSuccessful ) const
{
	// broadcast
	if ( InIsSuccessful
		&& OnFindSessionCompleteDelegate.IsBound()
		&& CachedSessionSearch.IsValid()
		&& CachedSessionSearch->SearchState == EOnlineAsyncTaskState::Done )
	{
		LOG_N( R4Log, TEXT("Find Session Complete. Num : [%d]."), CachedSessionSearch->SearchResults.Num() );
		OnFindSessionCompleteDelegate.Broadcast( CachedSessionSearch->SearchResults );
	}
}

/**
 * Join Session Complete. URL로 travel
 */
void UR4GameInstance::_OnJoinGameSessionComplete( FName InSessionName, EOnJoinSessionCompleteResult::Type InType )
{
	if( InType != EOnJoinSessionCompleteResult::Success )
	{
		LOG_WARN( R4Log, TEXT("Join Session Failed. Destroy Session.") );
		_DestroyGameSession();
		return;
	}
	
	FString address;

	IOnlineSubsystem* onlineSubsystem = IOnlineSubsystem::Get();
	if ( onlineSubsystem != nullptr )
	{
		IOnlineSessionPtr sessionInterface = onlineSubsystem->GetSessionInterface();
		if ( sessionInterface.IsValid() && sessionInterface->GetResolvedConnectString( NAME_GameSession, address ) )
		{
			APlayerController* playerController = GetFirstLocalPlayerController();
			if ( IsValid( playerController ) )
			{
				LOG_N( R4Log, TEXT("Join Session Complete. Try Travel to : [%s]"), *address );

				// travel to server, append net player name
				_AppendNetPlayerNameParam( address );
				playerController->ClientTravel( address, TRAVEL_Absolute );
			}
			return;
		}
	}
	
	LOG_WARN( R4Log, TEXT("Resolve connect string Failed. Destroy Session.") );
	_DestroyGameSession();
}

/**
 * singleton들을 등록
 */
void UR4GameInstance::_CreateSingletons()
{
	CREATE_SINGLETON( FDataTableManager, DataTableManager );
	CREATE_SINGLETON( FObjectPool, ObjectPool )
}

/**
 * singleton들을 정리
 */
void UR4GameInstance::_ClearSingletons()
{
	CLEAR_SINGLETON( DataTableManager )
	CLEAR_SINGLETON( ObjectPool )
}
