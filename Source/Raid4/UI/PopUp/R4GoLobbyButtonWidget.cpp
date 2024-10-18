// Fill out your copyright notice in the Description page of Project Settings.


#include "R4GoLobbyButtonWidget.h"

#include <Components/Button.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4GoLobbyButtonWidget)

void UR4GoLobbyButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if( IsValid( GoLobbyButton ) )
		GoLobbyButton->OnClicked.AddDynamic( this, &UR4GoLobbyButtonWidget::_OnClickGoLobbyButton );
}

/**
 *	_OnClickGoLobbyButton Lobby
 *	Game Instance를 통해 로비로 이동.
 */
void UR4GoLobbyButtonWidget::_OnClickGoLobbyButton()
{
	UR4GameInstance* gameInstance = GetGameInstance<UR4GameInstance>();
	if( IsValid( gameInstance ) )
		gameInstance->TravelToLobby();
}