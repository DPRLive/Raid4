// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4DetectorInterface.h"
#include "../../Core/ObjectPool/PoolableActor.h"
#include "../R4DetectStruct.h"

#include "R4Detector_Active.generated.h"

class UShapeComponent;

/**
 * Capsule, Box, Sphere 등 UShapeComponent 기반으로 하는 Comp를 통해 Detect를 실행.
 * No Auth에서 Detect 시, Authority측과 동기화.
 * 해당 Actor에 멤버로 등록 된 UShapeComponent 들은 모두 파싱되어 Detect에 이용되며,
 * 중첩 적용. ( 2개의 Shape가 해당 Actor의 멤버로 등록되고, 임의의 객체가 두개에 동시에 겹치면 두번 탐지됨 )
 * 기본적으로 Disable 상태이며, Execute Detect를 통해 Detect를 실행함.
 */
UCLASS( Abstract, HideCategories = ( Tick, Rendering, Actor, Input, HLOD, Physics, LevelInstance, WorldPartition, DataLayers ))
class RAID4_API AR4Detector_Active : public APoolableActor,
										public IR4DetectorInterface
{
	GENERATED_BODY()

public:
	AR4Detector_Active();
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// ~ Begin APoolableActor
	virtual void PostInitPoolObject() override {}
	virtual void PreReturnPoolObject() override;
	// ~ End APoolableActor
	
	// ~ Begin IR4DetectorInterface
	FORCEINLINE virtual FOnDetectDelegate& OnBeginDetect() override { return OnBeginDetectDelegate; }
	FORCEINLINE virtual FOnDetectDelegate& OnEndDetect() override { return OnEndDetectDelegate; }
	virtual void ExecuteDetect( AActor* InRequestActor, const FTransform& InOrigin, const FR4DetectDesc& InDetectDesc ) override;
	// ~ End IR4DetectorInterface

protected:
	// BP에서 ExecuteDetect시 확장을 위한 제공, Call only authority
	UFUNCTION( BlueprintImplementableEvent, Category = "Detect", meta=(DisplayName = "ExecuteDetect") )
	void BP_ExecuteDetect( AActor* InRequestActor, const FTransform& InOrigin, const FR4DetectDesc& InDetectDesc );

	// BP에서 OnBeginDetect시 확장을 위한 제공
	UFUNCTION( BlueprintImplementableEvent, Category = "Detect", meta=(DisplayName = "OnBeginDetect") )
	void BP_OnBeginDetect( const FR4DetectResult& InDetectResult );

	// BP에서 OnEndDetect시 확장을 위한 제공
	UFUNCTION( BlueprintImplementableEvent, Category = "Detect", meta=(DisplayName = "OnEndDetect") )
	void BP_OnEndDetect( const FR4DetectResult& InDetectResult );

	// BP에서 TearDown시 확장을 위한 제공, Call only authority
	UFUNCTION( BlueprintImplementableEvent, Category = "Detect", meta=(DisplayName = "TearDownDetect") )
	void BP_TearDownDetect();
	
private:
	// Life Time 설정
	void _SetLifeTime( float InLifeTime );
	
	// 멤버로 등록된 Shape Comp Begin Overlap 시 호출
	UFUNCTION()
	void _OnBeginShapeOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	// 멤버로 등록된 Shape Comp End Overlap 시 호출
	UFUNCTION()
	void _OnEndShapeOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

	// Detect 정리
	void _TearDownDetect();
	
	// Collision On Off
	UFUNCTION()
	void _OnRep_NoAuthCollisionInfo();
	
private:
	// Replicate시, No - Authority (Client) 환경에서도 Detect를 진행할지 설정.
	UPROPERTY( EditAnywhere, Category = "Replication|Detect" )
	uint8 bDetectOnNoAuthority:1;
	
	// Replicate시, No Authority에게 Enable Collision 상태 전송
	UPROPERTY( Transient, ReplicatedUsing = _OnRep_NoAuthCollisionInfo )
	FR4NoAuthDetectEnableInfo NoAuthCollisionEnableInfo;
	
	// Detect 시작 시 Broadcast
	FOnDetectDelegate OnBeginDetectDelegate;

	// Detect 종료 시 Broadcast.
	FOnDetectDelegate OnEndDetectDelegate;

	// Detect 생명주기를 위한 Timer
	FTimerHandle LifeTimerHandle;

	// Request를 요청한 Actor 캐싱.
	UPROPERTY( Replicated, Transient, VisibleInstanceOnly )
	TWeakObjectPtr<AActor> CachedRequestActor;

	// 생명 횟수 카운트. 해당 횟수 만큼 hit 시 Life Time이 다 안되어도 삭제. 0일 경우 카운트 하지 않음.
	UPROPERTY( EditAnywhere, Category = "Detect" )
	uint32 DetectLifeCount;
	
	// Hit count 캐싱
	uint32 CachedNowDetectLifeCount;
};
