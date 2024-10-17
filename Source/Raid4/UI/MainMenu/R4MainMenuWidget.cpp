// Fill out your copyright notice in the Description page of Project Settings.


#include "R4MainMenuWidget.h"
#include "R4SessionEntryWidget.h"
#include "../../Core/R4GameInstance.h"

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
	bool isLanMatch = true;
	if ( IsValid( IsLanMatchCheckBox ) )
		isLanMatch = IsLanMatchCheckBox->IsChecked();
	
	UR4GameInstance* gameInstance = GetGameInstance<UR4GameInstance>();
	if ( IsValid( gameInstance ) )
		gameInstance->CreateGameSession( isLanMatch );
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
		
		UR4SessionEntryWidget* newSessionEntry = NewObject< UR4SessionEntryWidget >( this, SessionList->GetDefaultEntryClass() );
		if( !IsValid( newSessionEntry ) )
			continue;
		
		FR4SessionEntryInfo entryInfo;
		entryInfo.ResultIndex = idx;
		entryInfo.Name = InResult[idx].Session.OwningUserName;
		entryInfo.NowPlayer = InResult[idx].Session.SessionSettings.NumPublicConnections - InResult[idx].Session.NumOpenPublicConnections;
		entryInfo.MaxPlayer = InResult[idx].Session.SessionSettings.NumPublicConnections;
		entryInfo.Ping = InResult[idx].PingInMs;
		
		newSessionEntry->SetSessionEntryInfo( entryInfo );
		
		// ListView에 추가
		SessionList->AddItem( newSessionEntry );
	}
}
