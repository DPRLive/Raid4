// Fill out your copyright notice in the Description page of Project Settings.

#include "R4SkillBase.h"
#include "../Buff/R4BuffReceiveInterface.h"
#include "../Detect/Detector/R4DetectorInterface.h"
#include "../Detect/R4DetectStruct.h"
#include "../Util/UtilStat.h"
#include "../Stat/R4TagStatQueryInterface.h"
#include "../Stat/R4StatStruct.h"

#include <Net/UnrealNetwork.h>
#include <GameFramework/Character.h>
#include <Components/SkeletalMeshComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4SkillBase)

UR4SkillBase::UR4SkillBase()
{
	// 필요 시에만 Ticking
    PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	BaseCoolDownTime = 0.f;
	CachedNextActivationServerTime = 0.f;
}

void UR4SkillBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UR4SkillBase, CachedNextActivationServerTime, COND_OwnerOnly);
}

/**
 *  스킬 사용이 가능한지 판단
 *  TODO : 현재 스킬 사용중인지 판단 추가
 */
bool UR4SkillBase::CanActivateSkill() const
{
	// CoolDown Time 체크
	return ( FMath::IsNearlyZero( GetSkillCooldownRemaining() ) );
}

/**
 *  현재 남은 Skill CoolDown Time을 반환.
 */
float UR4SkillBase::GetSkillCooldownRemaining() const
{
	float remainTime = CachedNextActivationServerTime - R4GetServerTimeSeconds( GetWorld() );
	return FMath::Max( 0.f, remainTime );
}

/**
 *  Skill CoolDown Time을 적용.
 *  @param InCoolDownTime : 적용할 CoolDownTime
 */
void UR4SkillBase::SetSkillCoolDownTime( float InCoolDownTime )
{
	// 쿨타임 적용
	CachedNextActivationServerTime = InCoolDownTime + R4GetServerTimeSeconds( GetWorld() );
}

/**
 *  현재 시점의 Skill의 CoolDownTime을 계산.
 *  @param InIsIgnoreReduction : 캐릭터의 CoolDownReduction을 무시하는지?
 */
float UR4SkillBase::GetSkillCoolDownTime( bool InIsIgnoreReduction ) const
{
	float cooldownTime = BaseCoolDownTime;

	if ( !InIsIgnoreReduction )
	{
		if ( IR4TagStatQueryInterface* owner = Cast<IR4TagStatQueryInterface>( GetOwner() ) )
		{
			if ( FR4StatInfo* coolDownReduction = owner->GetStatByTag( TAG_STAT_NORMAL_CoolDownReduction ) )
				cooldownTime *= UtilStat::GetCoolDownReductionFactor( coolDownReduction->GetTotalValue() );
		}
	}
	
	return cooldownTime;
}

/**
 *  Detect 실행.
 *  DetectorNetFlag && owner role 에 맞춰서 생성.
 *  Replicated Detector일 시 Server에서만 Spawn 해야함.
 *  particle이 있으나 투사체 등 (시각적인 요소 + 위치 같이) 중요하지 않다면 굳이 Replicate 하지 않고 Local 생성하는걸 권장
 *  @param InDetectBuffInfo : Detector & Buff Info.
 */
void UR4SkillBase::ExecuteDetect( const FR4SkillDetectBuffWrapper& InDetectBuffInfo )
{
	if ( !ensureMsgf( IsValid( InDetectBuffInfo.DetectorInfo.DetectClass ), TEXT("DetectClass is nullptr.") ) )
		return;

	// Parse Net Flag
	bool bReplicate = InDetectBuffInfo.DetectorInfo.DetectClass.GetDefaultObject()->GetIsReplicated();
	bool bServerSpawn = ( InDetectBuffInfo.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Server) );
	bool bOwnerSpawn = ( InDetectBuffInfo.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Owner ) );
	bool bSimulatedSpawn = ( InDetectBuffInfo.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Simulated ) );
	
	// Replicated Detector일 시 Server에서만 Spawn 해야함.
	if ( !ensureMsgf( bReplicate ? ( bServerSpawn && !bOwnerSpawn && !bSimulatedSpawn ) : true,
			TEXT("Replicated Detector must be spawned only on Server") ) )
		return;

	// role flag && owner role이 일치하면 Spawn 
	if ( ( GetOwnerRole() == ROLE_Authority && bServerSpawn )
		|| ( GetOwnerRole() == ROLE_AutonomousProxy && bOwnerSpawn )
		|| ( GetOwnerRole() == ROLE_SimulatedProxy && bSimulatedSpawn ) )
		 _SpawnDetector( InDetectBuffInfo );
}

/**
 *  Spawn Skill Detector
 *  @param InDetectBuffInfo : Detect 정보
 */
void UR4SkillBase::_SpawnDetector( const FR4SkillDetectBuffWrapper& InDetectBuffInfo )
{
	TScriptInterface<IR4DetectorInterface> detector(nullptr);
	
	// Detector 생성
	detector = OBJECT_POOL( GetWorld() )->GetObject( InDetectBuffInfo.DetectorInfo.DetectClass );
	if ( !IsValid( detector.GetObject() ) || detector.GetInterface() == nullptr )
	{
		LOG_WARN( R4Skill, TEXT("Detector Spawn Failed.") );
		
		OBJECT_POOL( GetWorld() )->ReturnPoolObject( detector.GetObject() );
		return;
	}

	// Server이면, Buff Bind.
	if ( GetOwnerRole() == ROLE_Authority )
	{
		// Bind buffs. this capture, weak lambda 사용
		// bind OnBeginDetect buff
		detector.GetInterface()->OnBeginDetect().AddWeakLambda(this,
			[this, &buffs = InDetectBuffInfo.OnBeginDetectBuffs]( const FR4DetectResult& InDetectResult )
		{
			_Server_ApplyDetectBuffs( InDetectResult.DetectedActor.Get(), buffs );
		});
	
		// bind OnEndDetect buff, damage
		detector.GetInterface()->OnEndDetect().AddWeakLambda(this,
		[this, &buffs = InDetectBuffInfo.OnEndDetectBuffs]( const FR4DetectResult& InDetectResult )
		{
			_Server_ApplyDetectBuffs( InDetectResult.DetectedActor.Get(), buffs );
		});
	}

	// origin 설정
	FTransform origin;
	if ( IsValid(GetOwner()) )
	{
		origin = GetOwner()->GetActorTransform();

		// Attach to mesh?
		if( InDetectBuffInfo.DetectorInfo.bAttachToMesh )
		{
			ACharacter* parentActor = Cast<ACharacter>( GetOwner() );
			USkeletalMeshComponent* parentSkel = parentActor ? parentActor->GetMesh() : nullptr;

			origin = _AttachDetectorToTargetMesh( Cast<AActor>( detector.GetObject() ), parentSkel, InDetectBuffInfo.DetectorInfo.MeshSocketName );
		}
	}

	// Execute Detect
	detector.GetInterface()->ExecuteDetect( origin, InDetectBuffInfo.DetectorInfo.DetectDesc );
}

/**
 *  Detector를 Target의 Mesh에 부착
 *  @param InDetector : attach될 Detector
 *  @param InTargetMesh : attach할 TargetMesh
 *  @param InSocketName : TargetMesh의 Socket Name
 *  @return : Attach한 Socket의 Transform
 */
FTransform UR4SkillBase::_AttachDetectorToTargetMesh( AActor* InDetector, USkeletalMeshComponent* InTargetMesh, const FName& InSocketName )
{
	if ( !IsValid( InDetector ) || !IsValid( InTargetMesh ) )
	{
		LOG_WARN( R4Skill, TEXT("InDetector or InTargetMesh is invalid."));
		return FTransform::Identity;
	}
	
	// parent skeleton에 부착
	InDetector->AttachToComponent( InTargetMesh, FAttachmentTransformRules::KeepWorldTransform, InSocketName );
	return InTargetMesh->GetSocketTransform( InSocketName );
}

/**
 *  특정한 버프들을 적용. Detect Buff는 Server Only.
 *  @param InVictim : Buff 적용 시 Victim 대상
 *  @param InSkillBuffInfos : 입힐 SkillBuff 들
 */
void UR4SkillBase::_Server_ApplyDetectBuffs( AActor* InVictim, const TArray<FR4SkillDetectBuffInfo>& InSkillBuffInfos ) const
{
	if ( !ensureMsgf( GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server") ) )
		return;
	
	for ( const FR4SkillDetectBuffInfo& buffInfo : InSkillBuffInfos )
	{
		// buff net flag에 맞춰서 적용
		AActor* target = nullptr;

		// 버프 입힐 대상 판정
		if ( buffInfo.Target == ETargetType::Instigator ) // 나를 대상으로 하는 버프
			target = GetOwner();
		else if ( buffInfo.Target == ETargetType::Victim ) // 탐지 대상에게 적용하는 버프
			target = InVictim;

		// 버프 적용
		if ( IR4BuffReceiveInterface* victim = Cast< IR4BuffReceiveInterface >( target ) )
			victim->ReceiveBuff( GetOwner(), buffInfo.BuffClass, buffInfo.BuffSetting );
	}
}