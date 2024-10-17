// Fill out your copyright notice in the Description page of Project Settings.


#include "R4LobbyGameMode.h"
#include "../Controller/R4PlayerController.h"
#include "../PlayerState/R4PlayerStateInterface.h"

#include <GameFramework/PlayerState.h>
#include <Kismet/GameplayStatics.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4LobbyGameMode)

AR4LobbyGameMode::AR4LobbyGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

/**
 *	Main Level로 이동할 수 있는지 확인.
 *	현재 접속한 캐릭터들이 캐릭터 선택을 모두 진행 했는지 확인
 */
bool AR4LobbyGameMode::CanTravelMainLevel() const
{
	for ( const auto& controller : CachedPlayerControllers )
	{
		if ( !IsValid( controller.Get() ) )
			continue;
		
		if( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( controller->PlayerState ) )
		{
			if ( playerState->GetCharacterId() == DTConst::G_InvalidPK )
				return false;
		}
	}

	return true;
}

/**
 *	캐릭터 선택 요청 처리.
 *	Player State에 저장.
 */
void AR4LobbyGameMode::RequestCharacterPick( APlayerController* InReqController, int32 InCharacterId )
{
	if( InCharacterId == DTConst::G_InvalidPK || !IsValid( InReqController ) )
		return;

	// 이미 선택이 된 상태이면, 무시
	if ( IsValid( InReqController ) )
	{
		if( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( InReqController->PlayerState ) )
		{
			if ( playerState->GetCharacterId() != DTConst::G_InvalidPK )
				return;
		}
	}
	
	// 중복 확인
	for ( const auto& controller : CachedPlayerControllers )
	{
		if ( !IsValid( controller.Get() ) || controller == InReqController )
			continue;
		
		if( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( controller->PlayerState ) )
		{
			if ( playerState->GetCharacterId() == InCharacterId )
				return;
		}
	}

	// Player State에 Push
	if( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( InReqController->PlayerState ) )
		playerState->SetCharacterId( InCharacterId );
}

/**
 *	seamless / post login 시 호출.
 */
void AR4LobbyGameMode::HandleStartingNewPlayer_Implementation( APlayerController* InNewPlayer )
{
	Super::HandleStartingNewPlayer_Implementation( InNewPlayer );

	if ( !IsValid( InNewPlayer ) )
		return;
	
	// 접속한 Player 캐싱
	CachedPlayerControllers.Emplace( InNewPlayer );
}

/**
 *	seamless / post login 시 호출.
 */
FString AR4LobbyGameMode::InitNewPlayer( APlayerController* InNewPlayerController, const FUniqueNetIdRepl& InUniqueId, const FString& InOptions, const FString& InPortal )
{
	FString ret = Super::InitNewPlayer( InNewPlayerController, InUniqueId, InOptions, InPortal );

	if( !IsValid( InNewPlayerController ) )
		return ret;
	
	// 접속 url에 Player Name이 존재한다면, 파싱 후 설정
	// URL 옵션에서 Player Name을 추출 및 설정
	FString playerName = UGameplayStatics::ParseOption( InOptions, NetGame::G_PlayerNameParamKey );

	APlayerState* playerState = InNewPlayerController->GetPlayerState<APlayerState>();
	if ( !IsValid( playerState ) )
		return ret;

	playerState->SetPlayerName( playerName );
	return ret;
}
