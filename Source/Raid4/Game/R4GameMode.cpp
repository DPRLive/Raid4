// Fill out your copyright notice in the Description page of Project Settings.


#include "R4GameMode.h"
#include "R4GameState.h"
#include "../Controller/R4PlayerController.h"
#include "../Character/R4CharacterBase.h"

#include <GameFramework/Pawn.h>
#include <GameFramework/SpectatorPawn.h>
#include <TimerManager.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4GameMode)

AR4GameMode::AR4GameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	
	NumPlayersToStartGame = 0;
	ToSpectatorDelay = 3.f;
}

/**
 *	seamless / post login 시 호출.
 */
void AR4GameMode::HandleStartingNewPlayer_Implementation( APlayerController* InNewPlayer )
{
	Super::HandleStartingNewPlayer_Implementation( InNewPlayer );

	CachedPlayers.Emplace( InNewPlayer );

	// 일정 인원 도달 시, Start Match
	if( CachedPlayers.Num() >= NumPlayersToStartGame )
		StartMatch();
}

/**
 *	게임 시작
 */
void AR4GameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AR4GameState* gameState = GetGameState<AR4GameState>();
	if ( !IsValid( gameState ) )
	{
		LOG_ERROR( R4Log, TEXT("Game State is nullptr.") );
		return;
	}
	
	// Game State로 부터 Game 상태 업데이트 수신
	gameState->OnUpdateGameState.AddUObject( this, &AR4GameMode::_CheckMatchState );
	
	// Player별 사용 중인 Character 정보 캐싱
	for( auto& [controller, pawn] : CachedPlayers )
	{
		pawn = controller->GetPawn();

		if( IR4DamageReceiveInterface* damageablePawn = Cast<IR4DamageReceiveInterface>( pawn ) )
			damageablePawn->OnDead().AddDynamic( this, &AR4GameMode::_PlayerDead );
	}
}

/**
 *	Player 사망, 지정된 시간 후 ASpectatorPawn으로 변경.
 */
void AR4GameMode::_PlayerDead( AActor* InDeadActor )
{
	if( APawn* pawn = Cast<APawn>( InDeadActor ) )
	{
		FTimerHandle handle;
		GetWorldTimerManager().SetTimer( handle, [deadPawn = TWeakObjectPtr<APawn>( pawn ) ]()
		{
			if( !deadPawn.IsValid() )
				return;

			APlayerController* controller = deadPawn->GetController<APlayerController>();
			if( !IsValid( controller ) || !IsValid( controller->GetWorld() ))
				return;

			ASpectatorPawn* spectatorPawn = controller->GetWorld()->SpawnActor<ASpectatorPawn>( ASpectatorPawn::StaticClass(), deadPawn->GetActorTransform() );
			if( IsValid( spectatorPawn ) )
			{
				controller->Possess( spectatorPawn );
				deadPawn->Destroy();
			}
		}, ToSpectatorDelay, false );
	}
}

/**
 *	Player, NPC 중 하나가 다 죽으면 게임 종료
 */
void AR4GameMode::_CheckMatchState()
{
	if ( IsMatchInProgress() )
	{
		AR4GameState* gameState = GetGameState<AR4GameState>();
		if ( !IsValid( gameState ) )
		{
			LOG_ERROR( R4Log, TEXT("Game State is nullptr.") );
			return;
		}
		
		if( gameState->GetNumAlivePlayers() <= 0 || gameState->GetNumAliveNPCs() <= 0 )
			EndMatch();
	}
}
