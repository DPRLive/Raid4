// Fill out your copyright notice in the Description page of Project Settings.


#include "R4GameState.h"
#include "../Damage/R4DamageReceiveInterface.h"

#include <Net/UnrealNetwork.h>
#include <TimerManager.h>
#include <Blueprint/UserWidget.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4GameState)

AR4GameState::AR4GameState()
{
	PrimaryActorTick.bCanEverTick = false;
	GameEndDelay = 0.0f;
	CachedGameEndDelayTimer = FTimerHandle();
	CachedNumAlivePlayers = 0;
	CachedNumAliveNPCs = 0;
}

void AR4GameState::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME( AR4GameState, CachedNumAlivePlayers );
	DOREPLIFETIME( AR4GameState, CachedNumAliveNPCs );
}

void AR4GameState::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	OnUpdateGameState.Clear();
	
	Super::EndPlay( EndPlayReason );
}

/**
 *	On Match Start
 */
void AR4GameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if ( HasAuthority() )
	{
		// Server에서 Level에 배치된 Player, NPC의 죽음 알림 수신 및 관리
		for( APawn* pawn : TActorRange<APawn>( GetWorld() ) )
		{
			if ( !IsValid( pawn ) )
				continue;

			IR4DamageReceiveInterface* damageablePawn = Cast<IR4DamageReceiveInterface>( pawn );
			if ( damageablePawn == nullptr )
				continue;
			
			if ( pawn->IsPlayerControlled() )
			{
				// Player
				damageablePawn->OnDead().AddDynamic( this, &AR4GameState::_PlayerDead );
				CachedNumAlivePlayers++;
			}
			else
			{
				// NPC
				damageablePawn->OnDead().AddDynamic( this, &AR4GameState::_NPCDead );
				CachedNumAliveNPCs++;
			}
		}

		_UpdateGameState();
	}
}

/**
 *	On Match End
 */
void AR4GameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	GetWorldTimerManager().SetTimer( CachedGameEndDelayTimer, [thisPtr = TWeakObjectPtr<AR4GameState>(this)]()
	{
		if( thisPtr.IsValid() )
			thisPtr->_AfterEndGame();
		
	}, GameEndDelay, false );
}

/**
 *	Player 사망
 */
void AR4GameState::_PlayerDead( AActor* InDeadActor )
{
	CachedNumAlivePlayers--;
	_UpdateGameState();
}

/**
 *	NPC
 */
void AR4GameState::_NPCDead( AActor* InDeadActor )
{
	CachedNumAliveNPCs--;
	_UpdateGameState();
}

/**
 *	Game State Update
 */
void AR4GameState::_UpdateGameState() const
{
	if( OnUpdateGameState.IsBound() )
		OnUpdateGameState.Broadcast();
}

/**
 *	Game State Update
 */
void AR4GameState::_AfterEndGame()
{
	UWorld* world = GetWorld();
	if( !IsValid( world ) )
		return;

	APlayerController* controller = world->GetFirstPlayerController();
	if( !IsValid( controller ) )
		return;
	
	// 종료 UI
	TSubclassOf<UUserWidget> endWidget = ( CachedNumAlivePlayers > CachedNumAliveNPCs )
											   ? PlayerWinWidget
											   : PlayerDeFeatWidget;
	UUserWidget* widget = CreateWidget<UUserWidget>( controller, endWidget );
	if ( !IsValid( widget ) )
		return;

	widget->AddToViewport();

	FInputModeUIOnly uiInput;
	controller->SetInputMode( uiInput );
	controller->SetShowMouseCursor( true );
	controller->FlushPressedKeys();
}
