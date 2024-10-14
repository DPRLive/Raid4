// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerController.h"
#include "../Game/R4CharacterPickGameModeInterface.h"

#include <GameFramework/GameStateBase.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerController)

AR4PlayerController::AR4PlayerController()
{
}

/**
 *	캐릭터 선택 요청
 *	@param InCharacterId : 선택할 Character의 DT PK
 */
void AR4PlayerController::RequestCharacterPick( int32 InCharacterId )
{
	if( InCharacterId != DTConst::G_InvalidPK )
		_ServerRPC_RequestCharacterPick( InCharacterId );
}

/**
 *	캐릭터 선택 요청 Server RPC
 *	@param InCharacterId : 선택할 Character의 DT PK
 */
void AR4PlayerController::_ServerRPC_RequestCharacterPick_Implementation( int32 InCharacterId )
{
	// Game Mode에 요청
	AGameModeBase* gameMode = GetWorld() ? GetWorld()->GetAuthGameMode() : nullptr;
	if( IR4CharacterPickGameModeInterface* gameModeInter = Cast<IR4CharacterPickGameModeInterface>( gameMode ) )
		gameModeInter->RequestCharacterPick( this, InCharacterId );
}