// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Core/ObjectPool/PoolableActor.h"
#include "R4ParticleActor.generated.h"

/**
 *	복잡한 Particle을 BP로 확장하여 스폰하기 위한 Actor.
 */
UCLASS( Abstract )
class RAID4_API AR4ParticleActor : public APoolableActor
{
	GENERATED_BODY()

public:
	AR4ParticleActor();

public:
	// ~ Begin APoolableActor
	virtual void PostInitPoolObject() override;
	virtual void PreReturnPoolObject() override;
	// ~ End APoolableActor

	// Particle을 Execute.
	virtual void Execute( const FTransform& InOrigin );

protected:
	// Particle을 정리
	virtual void TearDown();

	// BP에서 Execute시 확장을 위한 제공
	UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName = "Execute") )
	void BP_Execute( const FTransform& InOrigin );

	// BP에서 TearDown시 확장을 위한 제공
	UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName = "TearDown") )
	void BP_TearDown( );
	
private:
	// Life Time. <= 0.f 일 시 1Tick만 실행.
	UPROPERTY( EditAnywhere )
	float LifeTime;

	// 생명 주기를 위한 LifeTimer Handle
	FTimerHandle LifeTimerHandle;
};
