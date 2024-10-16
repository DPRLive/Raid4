// Fill out your copyright notice in the Description page of Project Settings.


#include "R4SessionEntryWidget.h"
#include "../../Core/R4GameInstance.h"

#include <Components/Button.h>
#include <Components/TextBlock.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4SessionEntryWidget)

UR4SessionEntryWidget::UR4SessionEntryWidget( const FObjectInitializer& InObjectInitializer )
	: Super( InObjectInitializer )
{
	CachedSessionInfo = FR4SessionEntryInfo();
}

void UR4SessionEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if( IsValid( JoinButton ) )
		JoinButton->OnClicked.AddDynamic( this, &UR4SessionEntryWidget::_OnClickJoinButton );
}

void UR4SessionEntryWidget::NativeOnListItemObjectSet( UObject* InListItemObject )
{
	IUserObjectListEntry::NativeOnListItemObjectSet( InListItemObject );

	UR4SessionEntryWidget* sessionEntry = Cast<UR4SessionEntryWidget>( InListItemObject );
	if ( IsValid( sessionEntry ) )
		SetSessionEntryInfo( sessionEntry->GetSessionEntryInfo() );
}

/**
 *	Session Entry 설정
 */
void UR4SessionEntryWidget::SetSessionEntryInfo( const FR4SessionEntryInfo& InEntryInfo )
{
	if ( IsValid( NameText ) )
		NameText->SetText( FText::FromString( InEntryInfo.Name ) );

	if ( IsValid( NumPlayerText ) )
	{
		FString numPlayer = FString::Printf( TEXT( "%d / %d" ), InEntryInfo.NowPlayer, InEntryInfo.MaxPlayer );
		NumPlayerText->SetText( FText::FromString( numPlayer ) );
	}

	if ( IsValid( PingText ) )
		PingText->SetText( FText::FromString( FString::Printf( TEXT( "%d" ), InEntryInfo.Ping ) ) );

	CachedSessionInfo = InEntryInfo;
}

/**
 *	_OnClickJoinButton, 설정된 Session에 Join 후 travel.
 */
void UR4SessionEntryWidget::_OnClickJoinButton()
{
	UR4GameInstance* gameInstance = GetGameInstance<UR4GameInstance>();
	if ( IsValid( gameInstance ) )
		gameInstance->JoinGameSession( CachedSessionInfo.ResultIndex );
}
