// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4SkillStruct.h"

#include <Components/ActorComponent.h>

#include "R4SkillBase.generated.h"

class IR4DetectorInterface;
struct FR4DetectResult;

/**
 * Skill의 Base가 되는 클래스.
 * 스킬을 위한 기능들을 제공
 * < 상속하여 클래스를 제작 시 주의할 점 >
 * ( Skill 타이밍 체크 )
 * - Skill 사용을 적절히 체크하여 Skill CoolDownTime을 체크 및 설정 (SetSkillCoolDownTime())
 */
UCLASS( Abstract, HideCategories = (ComponentTick, Tags, Replication, ComponentReplication, Activation, Variable, Navigation, AssetUserData) )
class RAID4_API UR4SkillBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4SkillBase();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// 스킬 사용이 가능한지 판단
	virtual bool CanActivateSkill() const;

	// 현재 남은 Skill CoolDown Time을 반환.
	float GetSkillCooldownRemaining() const;
	
protected:
	// Skill CoolDown Time을 적용. 
	void SetSkillCoolDownTime( float InCoolDownTime );

	// 현재 시점의 Skill의 CoolDownTime을 계산.
	float GetSkillCoolDownTime( bool InIsIgnoreReduction ) const;
	
	// Detect 실행
	void ExecuteDetect( const FR4SkillDetectBuffWrapper& InDetectBuffInfo );

	// NetFlag와 현재 Comp의 Net 상태를 비교 ( ER4NetworkFlag )
	bool IsMatchNetFlag( uint8 InNetFlag ) const;
private:
	// Spawn Detector
	void _SpawnDetector( const FR4SkillDetectBuffWrapper& InDetectBuffInfo );
	
	// Detector를 Target의 Mesh에 부착
	FTransform _AttachDetectorToTargetMesh( AActor* InDetector, USkeletalMeshComponent* InTargetMesh, const FName& InSocketName );

	// 특정한 버프들을 적용. Detect Buff는 Server Only.
	void _Server_ApplyDetectBuffs( AActor* InVictim, const TArray<FR4SkillDetectBuffInfo>& InSkillBuffInfos ) const;

private:
	// 해당 스킬의 쿨타임. 몇초마다 사용이 가능한지?
	UPROPERTY( EditAnywhere, meta = (UIMin = 0.f, ClampMin = 0.f, AllowPrivateAccess = true) )
	float BaseCoolDownTime;

	// 해당 스킬을 다시 사용할 수 있는 시간
	UPROPERTY( Replicated, Transient, VisibleInstanceOnly )
	float CachedNextActivationServerTime;
};