// Fill out your copyright notice in the Description page of Project Settings.


#include "R4LobbyGameMode.h"
#include "../Controller/R4PlayerController.h"
#include "../PlayerState/R4PlayerStateInterface.h"

#include <GameFramework/PlayerState.h>

#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4LobbyGameMode)

AR4LobbyGameMode::AR4LobbyGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	bUseSeamlessTravel = true;
}

void AR4LobbyGameMode::TravelToMainGame() const
{
	// 현재 접속한 캐릭터들이 캐릭터 선택을 모두 진행 했는지 확인
	for ( const auto& controller : CachedPlayerControllers )
	{
		if ( !IsValid( controller.Get() ) )
			continue;
		
		if( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( controller->PlayerState ) )
		{
			if ( playerState->GetCharacterId() == DTConst::G_InvalidPK )
				return;
		}
	}

	UWorld* world = GetWorld();
	if ( !IsValid( GetWorld() ) )
	{
		LOG_WARN( R4Log, TEXT("GetWorld() is invalid.") );
		return;
	}

	// Seamless travel
	world->ServerTravel( MainGameLevel.GetAssetName(), true );
}

/**
 *	캐릭터 선택 요청 처리.
 */
void AR4LobbyGameMode::RequestCharacterSelection( APlayerController* InReqController, int32 InCharacterId )
{
	if( InCharacterId == DTConst::G_InvalidPK || !IsValid( InReqController ) )
		return;
	
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

	// 접속한 Player 캐싱
	CachedPlayerControllers.Emplace( InNewPlayer );

	// TODO : 우끼끼 테스트~~!
	if( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( InNewPlayer->PlayerState ) )
	{
		FTimerHandle handle;
		GetWorldTimerManager().SetTimer( handle, [this, InNewPlayer]()
		{
			RequestCharacterSelection( InNewPlayer, CachedPlayerControllers.Num() );
		}, 3.f, false);
	}
}
