// Fill out your copyright notice in the Description page of Project Settings.

#include "R4SkillBase.h"
#include "../Buff/R4BuffReceiveInterface.h"
#include "../Detect/Detector/R4DetectorInterface.h"
#include "../Detect/R4DetectStruct.h"
#include "../Util/UtilStat.h"
#include "../Stat/R4TagStatQueryInterface.h"
#include "../Stat/R4StatStruct.h"
#include "../Calculator/Origin/R4OriginCalculatorInterface.h"

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
	bEnableSkill = true;
}

void UR4SkillBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UR4SkillBase, CachedNextActivationServerTime, COND_OwnerOnly);
}

void UR4SkillBase::BeginPlay()
{
	Super::BeginPlay();

	// 스킬 시작 / 종료 시 버프 적용
	// this capture, use weak lambda
	OnBeginSkill.AddWeakLambda( this, [this]()
	{
		// 나에게 버프 적용
		for( const auto& buff : OnBeginSkillBuffs )
			ApplySkillBuff(buff);
	} );

	OnEndSkill.AddWeakLambda( this, [this]()
	{
		// 나에게 버프 적용
		for( const auto& buff : OnEndSkillBuffs )
			ApplySkillBuff(buff);
	} );
}

/**
 *  End Play 시 Delegate Clear
 */
void UR4SkillBase::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	OnBeginSkill.Clear();
	OnEndSkill.Clear();
	OnSetSkillCooldown.Clear();
	
	Super::EndPlay( EndPlayReason );
}

/**
 *  스킬 사용이 가능한지 판단
 */
bool UR4SkillBase::CanActivateSkill() const
{
	// CoolDown Time , Enable 여부 체크
	return ( bEnableSkill && FMath::IsNearlyZero( GetSkillCooldownRemaining() ) );
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

	if( OnSetSkillCooldown.IsBound() )
		OnSetSkillCooldown.Broadcast();
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
 *  Replicated Detector일 시 Server에서만 Spawn 해야함.
 *  particle이 있으나 투사체 등 (시각적인 요소 + 위치 같이) 중요하지 않다면 굳이 Replicate 하지 않고 Local 생성하는걸 권장
 *  @param InDetectBuffInfo : Detector & Buff Info.
 */
void UR4SkillBase::ExecuteDetect( const FR4SkillDetectBuffWrapper& InDetectBuffInfo )
{
	if ( !ensureMsgf( IsValid( InDetectBuffInfo.DetectorInfo.DetectClass ), TEXT("DetectClass is nullptr.") ) )
		return;

	_SpawnDetector( InDetectBuffInfo );
}

/**
 *  NetFlag와 현재 Comp의 Net 상태를 비교 ( ER4NetworkFlag )
 *  @param InNetFlag : 비교할 Net Flag 정보 ( ER4NetworkFlag )
 *  @return : NetFlag와 Match하여 현재 Comp의 Net 상태와 맞으면 true 리턴
 */
bool UR4SkillBase::IsMatchNetFlag( uint8 InNetFlag ) const
{
	// Parse Net Flag
	bool bLocalMatch = false;
	if ( APawn* owner = Cast<APawn>( GetOwner() ) )
		bLocalMatch = ( InNetFlag & static_cast<uint8>( ER4NetworkFlag::Local ) ) && owner->IsLocallyControlled();
	
	bool bServerMatch = ( InNetFlag & static_cast<uint8>( ER4NetworkFlag::Server) ) && ( GetOwnerRole() == ROLE_Authority );
	
	bool bSimulatedMatch = (InNetFlag & static_cast<uint8>( ER4NetworkFlag::Simulated ) ) && ( GetOwnerRole() == ROLE_SimulatedProxy );

	return ( bLocalMatch || bServerMatch || bSimulatedMatch );
}

/**
 *  Owner에게 Skill Buff 적용
 *  @param InSkillBuff : 적용 할 SkillBuff 정보
 */
void UR4SkillBase::ApplySkillBuff( const FR4SkillBuffInfo& InSkillBuff ) const
{
	// Check Net Flag
	if ( !IsMatchNetFlag( InSkillBuff.BuffNetFlag ) )
		return;
			
	if ( IR4BuffReceiveInterface* victim = Cast<IR4BuffReceiveInterface>( GetOwner() ) )
		victim->ReceiveBuff( GetOwner(), InSkillBuff.BuffInfo.BuffClass, InSkillBuff.BuffInfo.BuffSetting );
}

/**
 *  Spawn Skill Detector
 *  @param InDetectBuffInfo : Detect 정보
 */
void UR4SkillBase::_SpawnDetector( const FR4SkillDetectBuffWrapper& InDetectBuffInfo )
{
	if ( !IsValid(GetOwner()) )
	{
		LOG_WARN( R4Skill, TEXT("Owner is invalid.") );
		return;
	}
	
	TScriptInterface<IR4DetectorInterface> detector( nullptr );
	
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
			[this, &buffs = InDetectBuffInfo.Server_OnBeginDetectBuffs]( const FR4DetectResult& InDetectResult )
		{
			_Server_ApplyDetectBuffs( InDetectResult.DetectedActor.Get(), buffs );
		});
	
		// bind OnEndDetect buff, damage
		detector.GetInterface()->OnEndDetect().AddWeakLambda(this,
		[this, &buffs = InDetectBuffInfo.Server_OnEndDetectBuffs]( const FR4DetectResult& InDetectResult )
		{
			_Server_ApplyDetectBuffs( InDetectResult.DetectedActor.Get(), buffs );
		});
	}

	// origin을 계산.
	FTransform origin = FTransform::Identity;
	
	if( !ensureMsgf( IsValid( InDetectBuffInfo.DetectorInfo.DetectorOriginCalculator ),
		TEXT("Detector Origin Calculator is nullptr.") ) )
	{
		return;
	}

	const UObject* cdo = InDetectBuffInfo.DetectorInfo.DetectorOriginCalculator->GetDefaultObject( true );
	const IR4OriginCalculatorInterface* originCalculator = Cast<IR4OriginCalculatorInterface>( cdo );
	if( originCalculator == nullptr )
	{
		LOG_WARN( R4Skill, TEXT("Origin Calculator is nullptr.") )
		return;
	}

	// Calculate origin.
	origin = originCalculator->CalculateOrigin( this, GetOwner() );

	// Attach ?
	if( InDetectBuffInfo.DetectorInfo.bAttachToMesh )
	{
		ACharacter* parentActor = Cast<ACharacter>( GetOwner() );
		USkeletalMeshComponent* parentSkel = parentActor ? parentActor->GetMesh() : nullptr;
		AActor* detectorActor = Cast<AActor>( detector.GetObject() );
		
		if ( IsValid( parentSkel ) && IsValid( detectorActor ) )
			detectorActor->AttachToComponent( parentSkel, FAttachmentTransformRules::KeepWorldTransform, InDetectBuffInfo.DetectorInfo.MeshSocketName );
	}
	
	// Execute Detect
	detector.GetInterface()->ExecuteDetect( GetOwner(), origin, InDetectBuffInfo.DetectorInfo.DetectDesc );
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
			victim->ReceiveBuff( GetOwner(), buffInfo.BuffInfo.BuffClass, buffInfo.BuffInfo.BuffSetting );
	}
}