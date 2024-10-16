// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterPickerWidget.h"
#include "../R4ImageIdButton.h"
#include "../R4ImageTextHorizontalBox.h"
#include "../../Controller/R4CharacterPickControllerInterface.h"
#include "../../Data/R4DTDataPushInterface.h"
#include "../../Data/Character/R4CharacterRow.h"
#include "../../Data/Character/R4CharacterSrcRow.h"
#include "../../PlayerState/R4PlayerStateInterface.h"
#include "../../Game/R4LobbyGameState.h"
#include "../../Game/R4LobbyGameMode.h"

#include <Components/Button.h>
#include <Components/HorizontalBox.h>
#include <Components/VerticalBox.h>
#include <GameFramework/PlayerState.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CharacterPickerWidget)

void UR4CharacterPickerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedPickedCharacterId = 0;

	if ( IsValid( CharacterFixButton ) )
		CharacterFixButton->OnClicked.AddDynamic( this, &UR4CharacterPickerWidget::_OnClickCharacterFixButton );

	if ( IsValid( GameStartButton ) )
		GameStartButton->OnClicked.AddDynamic( this, &UR4CharacterPickerWidget::_OnClickGameStartButton );

	// Player State Init
	_OnChangePlayerStateArray();

	AR4LobbyGameState* gameState = GetWorld() ? GetWorld()->GetGameState<AR4LobbyGameState>() : nullptr;
	if( IsValid( gameState ) )
	{
		// Player State의 변동 여부를 수신.
		gameState->OnChangePlayerArrayDelegate.AddUObject( this, &UR4CharacterPickerWidget::_OnChangePlayerStateArray );
	}
	
	// Server가 아니면, Hide Game Start Button.
	APlayerController* ownerController = GetOwningPlayer();
	if( !IsValid( ownerController ) || !ownerController->HasAuthority() )
	{
		if ( IsValid( GameStartButton ) )
			GameStartButton->SetVisibility( ESlateVisibility::Hidden );
	}

	// UHorizontalBox Setup
	for ( int32 key = DT_PC_BEGIN; key <= DT_PC_END; key++ )
		_CreateCharacterPickButton(key);
}

/**
 *	Character Pick, 미리보기 실행
 */
void UR4CharacterPickerWidget::_OnClickCharacterPortraitButton( int32 InCharacterId )
{
	if( IR4DTDataPushInterface* previewPawn = Cast<IR4DTDataPushInterface>( GetOwningPlayerPawn() ) )
	{
		previewPawn->PushDTData( InCharacterId );
		CachedPickedCharacterId = InCharacterId;
	}
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
 * create & add portrait button
 */
void UR4CharacterPickerWidget::_CreateCharacterPickButton( int32 InCharacterId )
{
	// Add Character Pick btn
	if ( UR4ImageIdButton* button = NewObject<UR4ImageIdButton>( this, CharacterPortraitButtonClass ) )
	{
		// image
		auto portrait = _GetCharacterPortrait( InCharacterId );
		if ( portrait != nullptr )
			button->SetImage( *_GetCharacterPortrait( InCharacterId ) );
		
		button->SetId( InCharacterId );
		button->OnImageIdButtonClickedDelegate.AddUObject( this, &UR4CharacterPickerWidget::_OnClickCharacterPortraitButton );

		if( IsValid( PortraitButtonBox ) )
			PortraitButtonBox->AddChild( button );
	}
}

/**
 *  On Change Player State. 시원~하게 밀어버리고 처음부터 다시 감시.
 */
void UR4CharacterPickerWidget::_OnChangePlayerStateArray()
{
	AR4LobbyGameState* gameState = GetWorld() ? GetWorld()->GetGameState<AR4LobbyGameState>() : nullptr;
	if( !IsValid( gameState ) )
		return;

	// Cached Player State Monitors 모두 제거
	for( auto& monitorInfo : CachedPlayerStateMonitors )
	{
		if( !monitorInfo.PlayerStatePtr.IsValid() )
			continue;
		
		if ( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( monitorInfo.PlayerStatePtr ) )
		{
			playerState->OnSetCharacterId().Remove( monitorInfo.CharacterIdHandle );
			playerState->OnSetPlayerName().Remove( monitorInfo.PlayerNameHandle );
		}
	}
	CachedPlayerStateMonitors.Reset();

	// PlayerPickBox 모두 제거
	PlayerPickBox->ClearChildren();

	// Portrait Box Btn 모두 활성화.
	if( IsValid( PortraitButtonBox ) )
	{
		TArray<UWidget*> portraitButtonBoxs = PortraitButtonBox->GetAllChildren();
		for( const auto& btn : portraitButtonBoxs )
		{
			if ( !IsValid( btn ) )
				continue;
		
			btn->SetIsEnabled( true );
			btn->SetVisibility( ESlateVisibility::Visible );
		}
	}
	
	// 생성
	for( const auto& playerState : gameState->PlayerArray )
	{
		if( IsValid( playerState ) )
			_AddPlayerStateMonitor( playerState );
	}
}

/**
 *  Player State 감시를 추가 + PlayerPickBox 동기화.
 *  @param InPlayerState : 감시할 Player State
 */
void UR4CharacterPickerWidget::_AddPlayerStateMonitor( APlayerState* InPlayerState )
{
	if ( !IsValid( InPlayerState ) )
		return;

	// 새로운 감시 추가
	if ( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( InPlayerState ) )
	{
		// PortraitNameHorizontalBox 생성
		UR4ImageTextHorizontalBox* widgetBox = NewObject<UR4ImageTextHorizontalBox>( this, PlayerPickBoxElemClass );
		if( !IsValid( widgetBox ) )
			return;

		if ( IsValid( PlayerPickBox ) )
			PlayerPickBox->AddChild( widgetBox );
		
		// monitor 생성
		CachedPlayerStateMonitors.Add( FR4PlayerStateMonitorInfo() );
		int32 monitorIdx = CachedPlayerStateMonitors.Num() - 1;

		CachedPlayerStateMonitors[monitorIdx].PlayerStatePtr = InPlayerState;
		
		// 이미 설정된 경우 대비, 감시 시작 전 호출
		_OnFixCharacterId( monitorIdx, playerState->GetCharacterId() );
		_OnSetPlayerName( monitorIdx, InPlayerState->GetPlayerName() );

		// Character ID 감시
		CachedPlayerStateMonitors[monitorIdx].CharacterIdHandle =
			playerState->OnSetCharacterId().AddWeakLambda( this,
		[this, monitorIdx]( int32 InId )
		{
			_OnFixCharacterId( monitorIdx, InId );
		} );

		// Player Name 감시
		CachedPlayerStateMonitors[monitorIdx].PlayerNameHandle =
			playerState->OnSetPlayerName().AddWeakLambda( this,
		[this, monitorIdx]( const FString& InName )
		{
			_OnSetPlayerName( monitorIdx, InName );
		} );
	}
}

/**
 *  PlayerState의 Character가 Fix 되었을 때 호출
 *  @param InMonitorIndex : Player State 감시 목록의 Index
 */
void UR4CharacterPickerWidget::_OnFixCharacterId( int32 InMonitorIndex, int32 InCharacterId )
{
	if ( !CachedPlayerStateMonitors.IsValidIndex( InMonitorIndex )
		|| InCharacterId == DTConst::G_InvalidPK )
		return;

	// Portrait Box Btn 비활성화.
	if ( IsValid( PortraitButtonBox ) )
	{
		TArray<UWidget*> portraitButtonBoxs = PortraitButtonBox->GetAllChildren();
		for( const auto& btn : portraitButtonBoxs )
		{
			if( UR4ImageIdButton* portraitBtn = Cast<UR4ImageIdButton>( btn ) )
			{
				if ( portraitBtn->GetId() == InCharacterId )
				{
					portraitBtn->SetIsEnabled( false );
					portraitBtn->SetVisibility( ESlateVisibility::HitTestInvisible );
				}
			}
		}
	}
	
	// PlayerPickBox에 Portrait push
	if ( IsValid( PlayerPickBox ) )
	{
		if( UWidget* pickBox = PlayerPickBox->GetChildAt( InMonitorIndex ) )
		{
			if ( UR4ImageTextHorizontalBox* portraitBox = Cast<UR4ImageTextHorizontalBox>( pickBox ) )
			{
				auto portrait = _GetCharacterPortrait( InCharacterId );
				if ( portrait != nullptr )
					portraitBox->SetImage( *_GetCharacterPortrait( InCharacterId ) );
			}
		}
	}
	
	// 어 내껀데
	if( CachedPlayerStateMonitors[InMonitorIndex].PlayerStatePtr == GetOwningPlayerState( ) )
	{
		// Character Fix 버튼 비활성화.
		if ( IsValid( CharacterFixButton ) )
		{
			CharacterFixButton->SetIsEnabled( false );
			CharacterFixButton->SetVisibility( ESlateVisibility::HitTestInvisible );
		}
		
		// PortraitButtonBox hit test off.
		if ( IsValid( PortraitButtonBox ) )
			PortraitButtonBox->SetVisibility( ESlateVisibility::HitTestInvisible );
	}
}

/**
 *  PlayerState의 PlayerName이 Set 되었을 때 호출
 *  @param InMonitorIndex : Player State 감시 목록의 Index
 */
void UR4CharacterPickerWidget::_OnSetPlayerName( int32 InMonitorIndex, const FString& InName )
{
	if ( !IsValid( PlayerPickBox ) )
		return;
	
	// PlayerPickBox에 Name push
	if( UWidget* pickBox = PlayerPickBox->GetChildAt( InMonitorIndex ) )
	{
		if ( UR4ImageTextHorizontalBox* portraitBox = Cast<UR4ImageTextHorizontalBox>( pickBox ) )
			portraitBox->SetText( InName );
	}
}

/**
 *  Character DT Key에 맞는 Portrait return
 */
const TSoftObjectPtr<UTexture>* UR4CharacterPickerWidget::_GetCharacterPortrait( int32 InCharacterId ) const
{
	const FR4CharacterRowPtr characterData( GetWorld(), InCharacterId );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterData is Invalid. PK : [%d]"), InCharacterId );
		return nullptr;
	}

	// Get Resource Pk
	const FR4CharacterSrcRowPtr characterSrcRow( GetWorld(), characterData->ResourceRowPK );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterSrcData is Invalid. PK : [%d]"), InCharacterId );
		return nullptr;
	}

	return &characterSrcRow->Portrait;
}
