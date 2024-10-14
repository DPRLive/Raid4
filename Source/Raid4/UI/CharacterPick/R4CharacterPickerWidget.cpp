// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterPickerWidget.h"
#include "R4CharacterPortraitButton.h"
#include "../../Controller/R4CharacterPickControllerInterface.h"
#include "../../Data/R4DTDataPushInterface.h"
#include "../../PlayerState/R4PlayerStateInterface.h"
#include "../../Game/R4LobbyGameState.h"
#include "../../Game/R4LobbyGameMode.h"

#include <Components/Button.h>
#include <Components/HorizontalBox.h>
#include <GameFramework/PlayerState.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CharacterPickerWidget)

void UR4CharacterPickerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedPickedCharacterId = 0;
	
	CharacterFixButton->OnClicked.AddDynamic( this, &UR4CharacterPickerWidget::_OnClickCharacterFixButton );
	GameStartButton->OnClicked.AddDynamic( this, &UR4CharacterPickerWidget::_OnClickGameStartButton );

	AR4LobbyGameState* gameState = GetWorld() ? GetWorld()->GetGameState<AR4LobbyGameState>() : nullptr;
	if( IsValid( gameState ) )
	{
		for( auto& playerState : gameState->PlayerArray )
			_OnNewPlayerState( playerState );
		
		// Player State의 변동 여부를 수신.
		gameState->OnChangePlayerArrayDelegate.AddUObject( this, &UR4CharacterPickerWidget::_OnChangePlayerStateArray );
	}
	
	// Server가 아니면, Hide Game Start Button.
	APlayerController* ownerController = GetOwningPlayer();
	if( !IsValid( ownerController ) || !ownerController->HasAuthority() )
		GameStartButton->SetVisibility( ESlateVisibility::Hidden );

	// UHorizontalBox Setup
	for ( int32 key = DT_PC_BEGIN; key <= DT_PC_END; key++ )
		_CreateCharacterPickButton(key);
}

/**
 *	Character Fix.
 */
void UR4CharacterPickerWidget::_OnClickCharacterFixButton()
{
	// Controller를 통해 Server로 요청.
	if( APlayerController* controller = GetOwningPlayer() )
	{
		if ( IR4CharacterPickControllerInterface* pickController = Cast<IR4CharacterPickControllerInterface>( controller ) )
			pickController->RequestCharacterPick( CachedPickedCharacterId );
	}
}

/**
 *	Game Start. ( Server )
 */
void UR4CharacterPickerWidget::_OnClickGameStartButton()
{
	AR4LobbyGameMode* lobbyGameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AR4LobbyGameMode>() : nullptr;
	if ( IsValid( lobbyGameMode ) )
		lobbyGameMode->TravelToMainGame();
}

/**
 *	 Character가 Fix 되었을 때 호출
 */
void UR4CharacterPickerWidget::_OnFixCharacterId( int32 InCharacterId )
{
	// Character Fix 버튼 비활성화.
	CharacterFixButton->SetIsEnabled( false );
	CharacterFixButton->SetVisibility( ESlateVisibility::HitTestInvisible );

	// CharacterBox hit test off.
	CharacterBox->SetVisibility( ESlateVisibility::HitTestInvisible );
}

/**
 *	Character Pick, 미리보기 실행
 */
void UR4CharacterPickerWidget::_OnClickCharacterPick( int32 InCharacterId )
{
	if( IR4DTDataPushInterface* previewPawn = Cast<IR4DTDataPushInterface>( GetOwningPlayerPawn() ) )
	{
		previewPawn->PushDTData( InCharacterId );
		CachedPickedCharacterId = InCharacterId;
	}
}

/**
 * create & add portrait button
 */
void UR4CharacterPickerWidget::_CreateCharacterPickButton( int32 InCharacterId )
{
	// Add Character Pick btn
	if ( UR4CharacterPortraitButton* button = NewObject<UR4CharacterPortraitButton>( this, CharacterPortraitButtonClass ) )
	{
		button->SetCharacterDTKey( InCharacterId );
		button->OnPortraitButtonClickedDelegate.AddUObject( this, &UR4CharacterPickerWidget::_OnClickCharacterPick );
		CharacterBox->AddChild( button );
	}
}

/**
 *  On Change Player State.
 */
void UR4CharacterPickerWidget::_OnChangePlayerStateArray()
{
}

/**
 *  On New Player State.
 */
void UR4CharacterPickerWidget::_OnNewPlayerState( APlayerState* InPlayerState )
{
	// Character ID가 Fix 되는지 수신
	if ( IR4PlayerStateInterface* playerStateInter = Cast<IR4PlayerStateInterface>( InPlayerState ) )
		playerStateInter->OnSetCharacterId().AddUObject( this, &UR4CharacterPickerWidget::_OnFixCharacterId );
}
