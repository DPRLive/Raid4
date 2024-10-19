// Fill out your copyright notice in the Description page of Project Settings.


#include "R4MainMenuWidget.h"
#include "R4SessionEntryWidget.h"
#include "../../Game/R4GameInstance.h"

#include <Components/Button.h>
#include <Components/CheckBox.h>
#include <Components/EditableText.h>
#include <Components/ListView.h>
#include <OnlineSessionSettings.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4MainMenuWidget)

UR4MainMenuWidget::UR4MainMenuWidget( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	MaxSessionSearchNum = 10;
}

void UR4MainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if ( IsValid( NewGameButton ) )
		NewGameButton->OnClicked.AddDynamic( this, &UR4MainMenuWidget::_OnClickNewGameButton );

	if ( IsValid( FindGameButton ) )
		FindGameButton->OnClicked.AddDynamic( this, &UR4MainMenuWidget::_OnClickFindGameButton );

	if ( IsValid( JoinByIPButton ) )
		JoinByIPButton->OnClicked.AddDynamic( this, &UR4MainMenuWidget::_OnClickJoinByIdButton );
	
	if ( IsValid( SessionList ) )
		SessionList->OnItemDoubleClicked().AddUObject( this, &UR4MainMenuWidget::_OnDoubleClickSessionEntry );

	UR4GameInstance* gameInstance = GetGameInstance<UR4GameInstance>();
	if ( !IsValid( gameInstance ) )
	{
		LOG_WARN( R4Log, TEXT("Game Instance is invalid.") );
		return;
	}

	gameInstance->OnFindSessionCompleteDelegate.AddUObject( this, &UR4MainMenuWidget::_OnFindSessionCompleted );
}

/**
 *	OnClickNewGameButton, New Session으로 Switch
 */
void UR4MainMenuWidget::_OnClickNewGameButton()
{
	// 이름 설정 안하면 안들여보내줌.
	FString playerName = _ParsePlayerName();
	if( playerName.Len() <= 0 )
		return;
	
	bool isLanMatch = true;
	if ( IsValid( IsLanMatchCheckBox ) )
		isLanMatch = IsLanMatchCheckBox->IsChecked();
	
	UR4GameInstance* gameInstance = GetGameInstance<UR4GameInstance>();
	if ( IsValid( gameInstance ) )
		gameInstance->CreateGameSession( isLanMatch, playerName );
}

/**
 *	_OnClickFindGameButton, GameInstance에 Session Find 요청.
 */
void UR4MainMenuWidget::_OnClickFindGameButton()
{
	bool isLanMatch = true;
	if ( IsValid( IsLanMatchCheckBox ) )
		isLanMatch = IsLanMatchCheckBox->IsChecked();
	
	UR4GameInstance* gameInstance = GetGameInstance<UR4GameInstance>();
	if ( IsValid( gameInstance ) )
		gameInstance->FindGameSession( isLanMatch, MaxSessionSearchNum );
}

/**
 *	Join by id button ( console command로 강제 open )
 */
void UR4MainMenuWidget::_OnClickJoinByIdButton()
{
	// 이름 설정 안하면 안들여보내줌.
	FString playerName = _ParsePlayerName();
	if( playerName.Len() <= 0 )
		return;
	
	UR4GameInstance* gameInstance = GetGameInstance<UR4GameInstance>();
	if ( IsValid( gameInstance ) )
		gameInstance->ForceTravel( _ParseIP(), playerName );
}

/**
 *	_OnDoubleClickSessionEntry (SessionList's widget)
 *	Session에 Join.
 */
void UR4MainMenuWidget::_OnDoubleClickSessionEntry( UObject* InObject ) const
{
	// 이름 설정 안하면 안들여보내줌.
	FString playerName = _ParsePlayerName();
	if( playerName.Len() <= 0 )
		return;
	
	UR4SessionEntryWidget* sessionEntry = Cast<UR4SessionEntryWidget>( InObject );
	if ( !IsValid( sessionEntry ) )
		return;
	
	UR4GameInstance* gameInstance = GetGameInstance<UR4GameInstance>();
	if ( IsValid( gameInstance ) )
		gameInstance->JoinGameSession( sessionEntry->GetSessionEntryInfo().ResultIndex, playerName );
}

/**
 *	Session 찾기 결과를 수신 및 List에 채우기
 */
void UR4MainMenuWidget::_OnFindSessionCompleted( const TArray<FOnlineSessionSearchResult>& InResult )
{
	if( !IsValid( SessionList ) )
		return;

	SessionList->ClearListItems();

	for ( int32 idx = 0; idx < InResult.Num(); idx++ )
	{
		if ( !InResult[idx].IsValid() )
			continue;
		
		UR4SessionEntryWidget* newSessionEntry = NewObject<UR4SessionEntryWidget>( this, SessionList->GetDefaultEntryClass() );
		if( !IsValid( newSessionEntry ) )
			continue;
		
		FR4SessionEntryInfo entryInfo;
		entryInfo.ResultIndex = idx;

		// Host Name Parsing
		if( auto it = InResult[idx].Session.SessionSettings.Settings.Find( NetGame::G_HostPlayerNameKey ) )
			entryInfo.Name = it->Data.ToString();
		else
			entryInfo.Name = InResult[idx].Session.OwningUserName;
		
		entryInfo.NowPlayer = InResult[idx].Session.SessionSettings.NumPublicConnections - InResult[idx].Session.NumOpenPublicConnections;
		entryInfo.MaxPlayer = InResult[idx].Session.SessionSettings.NumPublicConnections;
		entryInfo.Ping = InResult[idx].PingInMs;

		newSessionEntry->SetSessionEntryInfo( entryInfo );
		
		// ListView에 추가
		SessionList->AddItem( newSessionEntry );
	}
}

/**
 *	IP Parsing
 */
FString UR4MainMenuWidget::_ParseIP() const
{
	if( IsValid( IPText ) )
	{
		FString input = IPText->GetText().ToString();

		FString result;
		for (int32 i = 0; i < input.Len(); i++)
		{
			TCHAR now = input[i];

			// 숫자, . , :
			if (FChar::IsDigit(now) || now == TEXT('.') || now == TEXT(':'))
				result += now;
		}
		
		return result;
	}

	return FString();
}

/**
 *	Player Name Parsing
 *	PlayerNameText의 PlayerName을 영어, 숫자, 한글만 남기고 리턴.
 */
FString UR4MainMenuWidget::_ParsePlayerName() const
{
	if( IsValid( PlayerNameText ) )
	{
		FString input = PlayerNameText->GetText().ToString();

		FString result;
		for (int32 i = 0; i < input.Len(); i++)
		{
			TCHAR now = input[i];

			// 영어, 숫자, || 한글 ( 한글 범위 체크 (가 ~ 힣) )
			if ( FChar::IsAlnum( now ) || ( now >= 0xAC00 && now <= 0xD7A3 ) )
				result += now;
		}
		
		return result;
	}

	return FString();
}
