// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/DataAsset.h>
#include "R4DataAsset_PlayerCharacterDTKey.generated.h"

/**
 * Character DT 중, Player Character로 사용될 수 있는 PK를 설정.
 */
UCLASS()
class RAID4_API UR4DataAsset_PlayerCharacterDTKey : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 캐릭터 PK로 사용될 수 있는 PK의 시작 Number.
	UPROPERTY( EditAnywhere, Category = "Pawn" )
	int32 PlayerCharacterDTBeginKey = 0;

	// 캐릭터 PK로 사용될 수 있는 PK의 마지막 Number.
	UPROPERTY( EditAnywhere, Category = "Pawn" )
	int32 PlayerCharacterDTEndKey = 0;
};