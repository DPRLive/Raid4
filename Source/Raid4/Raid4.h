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

// 월드를 반환한다.
UWorld* R4GetWorld( UObject* InObject = nullptr );

// 서버 시간을 반환한다.
double R4GetServerTimeSeconds( UWorld* InWorld = nullptr );
