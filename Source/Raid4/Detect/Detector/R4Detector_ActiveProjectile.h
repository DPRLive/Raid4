// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4Detector_Active.h"
#include "R4Detector_ActiveProjectile.generated.h"

/**
 * 간단히 날아가는 Detector.
 */
UCLASS( Abstract, HideCategories = ( Tick, Rendering, Actor, Input, HLOD, Physics, LevelInstance, WorldPartition, DataLayers ))
class RAID4_API AR4Detector_ActiveProjectile : public AR4Detector_Active
{
	GENERATED_BODY()

public:
	AR4Detector_ActiveProjectile();

public:
	virtual void Tick( float DeltaTime ) override;

private:
	// 방향. ( 기존 Actor의 방향의 Relative로 적용 )
	UPROPERTY( EditAnywhere, Category = "Projectile" )
	FVector Direction;

	// 속도
	UPROPERTY( EditAnywhere, Category = "Projectile" )
	float Speed;
};
