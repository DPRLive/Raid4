// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4AnimSkillBase.h"
#include "../../Input/R4PlayerInputableInterface.h"
#include "../../Calculator/Origin/R4OriginProviderInterface.h"

#include "R4Skill_PlayerPoint.generated.h"

/**
 * 지점 선택을 위한 정보.
 */
USTRUCT()
struct FR4PointSkillAOEInfo
{
	GENERATED_BODY()
	
	FR4PointSkillAOEInfo()
	: RangeAOE( FR4AreaOfEffect() )
	, RangeRadius( 0.f )
	, PointAOE( FR4AreaOfEffect() )
	, PointRadius( 0.f )
	, RangeAOEInstance( nullptr )
	, PointAOEInstance( nullptr )
	{}
	
	// 범위 표시 Decal
	UPROPERTY( EditAnywhere )
	FR4AreaOfEffect RangeAOE;

	// 범위 반지름
	UPROPERTY( EditAnywhere )
	float RangeRadius;
	
	// 지점 표시 Decal
	UPROPERTY( EditAnywhere )
	FR4AreaOfEffect PointAOE;

	// Point 반지름
	UPROPERTY( EditAnywhere )
	float PointRadius;
	
	// 범위 표시 Decal Comp Instance
	UPROPERTY( Transient, VisibleInstanceOnly )
	TObjectPtr<UDecalComponent> RangeAOEInstance;

	// 지점 표시 Decal Comp Instance
	UPROPERTY( Transient, VisibleInstanceOnly )
	TObjectPtr<UDecalComponent> PointAOEInstance;
};

/**
 * 플레이어 지점 선택 스킬 ( 첫번째 사용 시 지점 선택, 두번째 사용 시 실제 스킬 사용 )
 * Detector의 Origin으로 UR4OriginCalculator_FromRequestObj를 사용하면, 선택된 지점을 기반 Origin을 사용함
 * TODO : 현재 범위 기반 계산만 가능한데, 다른 방식이 더 필요하면 Strategy로 확장?
 */
UCLASS( Blueprintable, NotBlueprintType, ClassGroup=(Skill) )
class RAID4_API UR4Skill_PlayerPoint : public UR4AnimSkillBase, public IR4PlayerInputableInterface, public IR4OriginProviderInterface
{
	GENERATED_BODY()

public:
	UR4Skill_PlayerPoint();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

public:
	// ~ Begin IR4PlayerInputableInterface
	virtual void OnInputStarted() override;
	virtual void OnInputTriggered() override {}
	virtual void OnInputCompleted() override {}
	// ~ End IR4PlayerInputableInterface

	// ~ Begin IR4OriginProviderInterface
	virtual FTransform GetOrigin( const UObject* InRequestObj, const AActor* InActor ) const override;
	// ~ End IR4OriginProviderInterface
	
	// 스킬을 Disable / Enable.
	virtual void SetSkillEnable( bool InIsEnable ) override;
protected:
	// Anim을 Play시작 시 호출.
	virtual void OnBeginSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, float InStartServerTime ) override;

	// Anim 종료 시 호출.
	virtual void OnEndSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, bool InIsInterrupted ) override;

	// 현재 Tick이 필요한 상태인지 return.
	virtual bool IsNeedTick() const override;

private:
	// 지점 선택 Trace
	void _TracePointing();

	// Pointing Setup
	void _SetupPointing();
	
	// Pointing 정리
	void _TearDownPointing() const;
	
	// 지점 지정, Decal도 유효하다면 같이 이동.
	void _SetTargetPointAndDecal( const FVector& InWorldLoc );

	// Server로 선택한 지점 전송
	UFUNCTION( Server, Reliable, WithValidation )
	void _ServerRPC_SendTargetPoint( const FVector_NetQuantize& InWorldLoc );

	// Target Point가 Server로 인해 확정 시 호출
	UFUNCTION()
	void _OnRep_TargetWorldPoint();
	
private:
	// 지점 선택 중 애니메이션
	UPROPERTY( Replicated, EditAnywhere, Category = "Skill|Anim" )
	FR4SkillAnimInfo PointingSkillAnimInfo;

	// 지점 선택 관련 정보
	UPROPERTY( EditAnywhere, Category = "Skill|Anim" )
	FR4PointSkillAOEInfo AOEInfo;
	
	// 지점 선택 시 적용할 버프
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillBuffInfo> OnBeginPointingBuffs;
	
	// 지점 선택 종료 시 적용할 Buff.
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillBuffInfo> OnEndPointingBuffs;
	
	// 공격 스킬 애니메이션, 지점 선택 완료 시 바로 작동
	UPROPERTY( Replicated, EditAnywhere, Category = "Skill|Anim" )
	FR4SkillAnimInfo AttackSkillAnimInfo;
	
	// Caching

	// 선택된 지점 ( Server )
	UPROPERTY( ReplicatedUsing = _OnRep_TargetWorldPoint, Transient, VisibleInstanceOnly )
	FVector_NetQuantize CachedTargetWorldPoint_Server;
	
	// 선택된 지점 ( Client )
	FVector CachedTargetWorldPoint_Client;
	
	// 현재 지점 선택 중인지
	uint8 CachedNowPointing:1;
	
	// Player Controller
	TWeakObjectPtr<APlayerController> CachedPlayerController;
};
