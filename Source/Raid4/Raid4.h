// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// PCH 모음
#include <CoreMinimal.h>
#include <Engine/Engine.h>
#include <EngineUtils.h>
#include <GameplayTags.h>

#include "Util/UtilEnum.h"
#include "Util/UtilPath.h"
#include "Util/UtilEffect.h"
#include "Common/R4Log.h"
#include "Common/R4Const.h"
#include "Common/R4Typedef.h"
#include "Common/R4Enum.h"
#include "Common/R4Delegate.h"
#include "Common/R4Struct.h"
#include "Data/R4DataAssetHelper.h"
#include "Tag/R4GameplayTag.h"
#include "Core/ObjectPool/ObjectPool.h"
#include "Game/R4GameInstance.h"
#include "Data/Character/R4DataAsset_PlayerCharacterDTKey.h"

// Defines
#define OBJECT_POOL(World) \
	R4GetGameInstance<UR4GameInstance>(World)->GetObjectPool()

#define DTManager(World) \
	R4GetGameInstance<UR4GameInstance>(World)->GetDataTableManager()

// Player Character DT Begin Key
#define DT_PC_BEGIN	\
	R4DataAssetHelper::GetDataAsset<UR4DataAsset_PlayerCharacterDTKey>( TEXT( "DA_PlayerCharacterDTKey" ) )->PlayerCharacterDTBeginKey

// Player Character DT End Key
#define DT_PC_END	\
	R4DataAssetHelper::GetDataAsset<UR4DataAsset_PlayerCharacterDTKey>( TEXT( "DA_PlayerCharacterDTKey" ) )->PlayerCharacterDTEndKey
	
// 월드를 반환한다.
UWorld* R4GetWorld( UObject* InObject = nullptr );

// 서버 시간을 반환한다.
double R4GetServerTimeSeconds( UWorld* InWorld = nullptr );

// GameInstance 반환
template<typename T>
T* R4GetGameInstance( UWorld* InWorld = nullptr )
{
	InWorld = InWorld ? InWorld : R4GetWorld(  );
	return InWorld->GetGameInstance<T>( );
}