// Copyright Epic Games, Inc. All Rights Reserved.

#include "Raid4.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Raid4, "Raid4" );

/**
 *	월드를 가져오기 거시기 한경우를 위한 월드 반환 함수.
 */
UWorld* R4GetWorld(UObject* InObject)
{
	if (InObject != nullptr)
		return InObject->GetWorld();
	
	if (FWorldContext* world = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport))
	{
		return world->World();
	}

	LOG_SCREEN(FColor::Red, TEXT( "Failed to Get World!!" ));
	return nullptr;
}
