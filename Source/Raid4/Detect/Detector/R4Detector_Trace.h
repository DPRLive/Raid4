// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4DetectorInterface.h"
#include "../../Core/ObjectPool/PoolableActor.h"
#include "../R4DetectStruct.h"

#include <PhysicsEngine/BodyInstance.h>

#include "R4Detector_Trace.generated.h"

/**
 * Trace를 기반으로 Detect를 수행하는 클래스.
 * No Auth에서 Detect 시, Authority측과 동기화.
 * 너무 짧은 시간이거나, 시작과 동시에 Trace를 진행하는 경우 Server->Client 통신 간 delay 된 만큼의 Trace가 무시 될 수 있음.
 * Interval의 경우, 최초 1회 실행 후 이후 시간을 체크하는 방식.
 */
UCLASS( Abstract, HideCategories = ( Tick, Rendering, Actor, Input, HLOD, Physics, LevelInstance, WorldPartition, DataLayers ) )
class RAID4_API AR4Detector_Trace : public APoolableActor,
										public IR4DetectorInterface
{
	GENERATED_BODY()

public:
	AR4Detector_Trace();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	// BP에서 TearDown시 확장을 위한 제공, Call only authority
	UFUNCTION( BlueprintImplementableEvent, Category = "Detect", meta=(DisplayName = "TearDownDetect") )
	void BP_TearDownDetect();
	
private:
	// Life Time 설정
	void _SetLifeTime( float InLifeTime );
	
	// Trace를 Delay에 맞춰 예약
	void _ExecuteTrace( float InEnableDelayedTime );
	
	// Trace Logic
	void _Trace( );

	// Detect 정리
	void _TearDownDetect();
	
	// No Auth Trace Info
	UFUNCTION()
	void _OnRep_NoAuthTraceInfo();
private:
	// Trace의 실행 타입
	UPROPERTY( EditAnywhere, Category = "Collision" )
	ER4TraceDetectExecutionType ExecutionType;

	// Trace를 체크할 간격시간.
	// 3초 LifeTime에 Interval 1초 일 시, 0, 1, 2 3번 사용됨. 
	UPROPERTY( EditAnywhere, Category="Collision", meta = ( ClampMin = 0.01f, UIMin = 0.01f,
		EditCondition = "ExecutionType == ER4TraceDetectExecutionType::Interval", EditConditionHides) )
	float DetectInterval;

	// Trace를 체크 시작까지의 Delay.
	UPROPERTY( EditAnywhere, Category="Collision", meta = ( ClampMin = 0.f, UIMin = 0.f ) )
	float Delay;
	
	// Detect에 사용할 모양 정보
	UPROPERTY( EditAnywhere, Category="Collision" )
	FR4TraceDetectShapeInfo ShapeInfo;

	// Detect에 사용할 Collision Response를 위한 BodyInstance
	UPROPERTY( EditAnywhere, Category="Collision", meta=(ShowOnlyInnerProperties, SkipUCSModifiedProperties, AllowPrivateAccess = true) )
	FBodyInstance BodyInstance;

	// Replicate시, No - Authority (Client) 환경에서도 Trace를 진행할지 설정.
	// Trace의 경우 1 Tick만 실행하므로, ExecutionType이 Immediately이거나,
	// Replicate가 Pkt Lag 때문에 늦게 진행된 경우 정해진 시간의 Trace가 무시 될 수 있음. 
	UPROPERTY( EditAnywhere, Category = "Replication|Detect" )
	uint8 bDetectOnNoAuthority:1;

	// Replicate시, No Authority에게 Trace 사용 상태 전송
	UPROPERTY( Transient, ReplicatedUsing = _OnRep_NoAuthTraceInfo )
	FR4NoAuthDetectEnableInfo NoAuthTraceEnableInfo;
	
	// Detect 시작 시 Broadcast
	FOnDetectDelegate OnBeginDetectDelegate;

	// Detect 종료 시 Broadcast. OneFrame Check의 경우 OnBegin과 동시에 End.
	FOnDetectDelegate OnEndDetectDelegate;

	// Detect 생명주기를 위한 Timer
	FTimerHandle LifeTimerHandle;

	// Detect Delay, Interval을 위한 Timer
	FTimerHandle TraceTimerHandle;

	// Request를 요청한 Actor 캐싱.
	UPROPERTY( Replicated, Transient, VisibleInstanceOnly )
	TWeakObjectPtr<AActor> CachedRequestActor;
	
	// Debug //
	// TODO : Debug 분리 ?
	
	// Debug 할 것인지 설정
	UPROPERTY( EditAnywhere, Category="Debug", meta =(AllowPrivateAccess = true) )
	uint8 bDrawDebug:1;

	// Debug를 얼마나 그릴것인지 설정
	UPROPERTY( EditAnywhere, Category="Debug", meta =(AllowPrivateAccess = true) )
	float DebugTime;
	
	// Debug Color 설정
	UPROPERTY( EditAnywhere, Category="Debug", meta =(AllowPrivateAccess = true) )
	FColor DebugColor;
};