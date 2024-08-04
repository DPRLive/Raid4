// Copyright Epic Games, Inc. All Rights Reserved.

#include "Raid4.h"

#include <GameFramework/GameStateBase.h>
#include <Modules/ModuleManager.h>

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Raid4, "Raid4" );

/**
 *	월드를 가져오기 거시기 한경우를 위한 월드 반환 함수.
 */
UWorld* R4GetWorld(UObject* InObject)
{
	UWorld* retWorld = nullptr;
	if (IsValid(InObject))
		retWorld = InObject->GetWorld();

	if(IsValid(GEngine) && retWorld == nullptr)
	{
		if (FWorldContext* world = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport))
		{
			retWorld = world->World();
		}
	}
	
	LOG_SCREEN(FColor::Red, TEXT( "Failed to Get World!!" ));
	return retWorld;
}

double R4GetServerTimeSeconds(UWorld* InWorld)
{
	UWorld* world = IsValid(InWorld) ? InWorld : R4GetWorld();

	if(AGameStateBase* gameState = (IsValid(world) ? world->GetGameState() : nullptr) ; IsValid(gameState))
		return gameState->GetServerWorldTimeSeconds();

	return -1.f;
}
