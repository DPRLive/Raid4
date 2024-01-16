// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// PCH 모음
#include <CoreMinimal.h>
#include <Engine/Engine.h>

#include "Common/Const.h"
#include "Common/Typedef.h"
#include "Common/R4Log.h"
#include "Util/UtilEnum.h"
#include "Common/R4Enum.h"
#include "Util/UtilPath.h"
#include "Util/UtilEffect.h"
#include "Data/DataAsset/R4DataAssetHelper.h"

// 월드를 반환한다.
UWorld* R4GetWorld( UObject* InObject = nullptr );

// SoftObject를 인스턴스로 만들어 반환한다.
template<typename T>
T* LoadSoftObjectSync(const TSoftObjectPtr<T>& InSoftObj)
{
	if(InSoftObj.IsNull())
	{
		LOG_WARN( R4Data, TEXT("SoftObjectPtr is null.") );
		return nullptr;
	}
	
	if(InSoftObj.IsPending())
		return InSoftObj.LoadSynchronous();
	
	return InSoftObj.Get();
}
