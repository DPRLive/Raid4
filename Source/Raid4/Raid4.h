// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// PCH 모음
#include <CoreMinimal.h>
#include <Engine/Engine.h>
#include <EngineUtils.h>

#include "Util/UtilEnum.h"
#include "Util/UtilPath.h"
#include "Util/UtilEffect.h"
#include "Common/Const.h"
#include "Common/Typedef.h"
#include "Common/R4Log.h"
#include "Common/R4Enum.h"
#include "Data/DataAsset/R4DataAssetHelper.h"

// 월드를 반환한다.
UWorld* R4GetWorld( UObject* InObject = nullptr );
