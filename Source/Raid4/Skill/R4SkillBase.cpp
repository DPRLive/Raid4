// Fill out your copyright notice in the Description page of Project Settings.

#include "R4SkillBase.h"
#include "../Buff/R4BuffReceiveInterface.h"
#include "../Damage/R4DamageReceiveInterface.h"
#include "../Detect/Detector/R4DetectorInterface.h"
#include "../Detect/R4DetectStruct.h"
#include "../Util/UtilDamage.h"
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
	CachedSkillDetectInfoCount = 0;
}

void UR4SkillBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UR4SkillBase, CachedNextActivationServerTime, COND_OwnerOnly);
}

void UR4SkillBase::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwnerRole() == ROLE_Authority)
		_Server_ParseSkillDetectInfo();
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

	if(!InIsIgnoreReduction)
	{
		if(IR4TagStatQueryInterface* owner = Cast<IR4TagStatQueryInterface>(GetOwner()) )
		{
			if(FR4StatInfo* coolDownReduction = owner->GetStatByTag(TAG_STAT_NORMAL_CoolDownReduction))
				cooldownTime *= UtilStat::GetCoolDownReductionFactor(coolDownReduction->GetTotalValue());
		}
	}
	
	return cooldownTime;
}

/**
 *  특정한 버프들을 적용.
 *  @param InVictim : Buff 적용 시 Victim 대상
 *  @param InSkillBuffInfos : 입힐 SkillBuff 들
 */
void UR4SkillBase::Server_ApplyBuffs( AActor* InVictim, const TArray<FR4SkillBuffInfo>& InSkillBuffInfos ) const
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server")))
		return;
	
	for(const FR4SkillBuffInfo& buffInfo : InSkillBuffInfos)
	{
		AActor* target = nullptr;

		// 버프 입힐 대상 판정
		if(buffInfo.Target == ETargetType::Instigator) // 나를 대상으로 하는 버프
			target = GetOwner();
		else if(buffInfo.Target == ETargetType::Victim) // 탐지 대상에게 적용하는 버프
			target = InVictim;

		// 버프 적용
		if( IR4BuffReceiveInterface* victim = Cast<IR4BuffReceiveInterface>(target) )
			victim->ReceiveBuff(GetOwner(), buffInfo.BuffClass, buffInfo.BuffSetting);
	}
}

/**
 *  특정한 데미지들을 적용.
 *  @param InVictim : Damage 적용 시 Victim 대상
 *  @param InSkillDamageInfos : 입힐 데미지들.
 */
void UR4SkillBase::Server_ApplyDamages( AActor* InVictim, const TArray<FR4SkillDamageInfo>& InSkillDamageInfos ) const
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server")))
		return;
	
	for(const FR4SkillDamageInfo& damageInfo : InSkillDamageInfos)
	{
		AActor* target = nullptr;

		// 데미지 입힐 대상 판정
		if(damageInfo.Target == ETargetType::Instigator) // 나를 대상으로 하는 데미지
			target = GetOwner();
		else if(damageInfo.Target == ETargetType::Victim) // 탐지 대상에게 적용하는 데미지
			target = InVictim;

		// 데미지 적용
		if( IR4DamageReceiveInterface* victim = Cast<IR4DamageReceiveInterface>(target) )
		{
			FR4DamageReceiveInfo damageRecvInfo = UtilDamage::CalculateDamageReceiveInfo( GetOwner(), target, damageInfo.DamageInfo );
			victim->ReceiveDamage( GetOwner(), damageRecvInfo );
		}
	}
}

/**
 *  Detect 실행
 *  Owner Client(Disable Collision), Server(+ Enable Collision) 경우에만 생성
 *  Owner Client : Visual 적인 요소가 필요한 경우 Dummy 생성, 후에 서버에서 생성되면 Dummy 제거
 *  Server : Collision을 포함한 실제 Detector 생성
 *  @param InDetectEffectInfo : 탐지 클래스 및 효과.
 */
void UR4SkillBase::ExecuteDetect( const FR4DetectEffectWrapper& InDetectEffectInfo )
{
	if ( !ensureMsgf( IsValid( InDetectEffectInfo.DetectInfo.DetectClass ), TEXT("DetectClass is nullptr.") ) )
		return;

	if( InDetectEffectInfo.DetectInfo.DetectorServerKey == Skill::G_InvalidDetectorKey )
	{
		LOG_WARN( R4Skill, TEXT("Detector Key Is Invalid. Check Replicate State."));
		return;
	}

	// Visual 적인 요소가 있는 경우 반드시 Replicate하여 Dummy를 생성해야하고
	// 아닌경우 Replicate를 금지하여 네트워크를 최적화
	if( !ensureMsgf( InDetectEffectInfo.DetectInfo.bHasVisual ==
		InDetectEffectInfo.DetectInfo.DetectClass.GetDefaultObject()->GetIsReplicated(),
		TEXT("visual detectors, it should be replicated. and if it is not visual, it should be not replicate.")) )
		return;

	// Simulated Proxy인 경우, 필요 없음.
	if( GetOwnerRole() == ROLE_SimulatedProxy )
		return;

	// Owner Client 이지만 Dummy가 필요한 경우 (Visual 적인 요소가 필요한 경우)
	if( GetOwnerRole() == ROLE_AutonomousProxy && InDetectEffectInfo.DetectInfo.bHasVisual )
	{
		_CreateDummyDetector( InDetectEffectInfo.DetectInfo );
		return;
	}

	// Server인 경우, 실제 Collision을 사용 & Effect 적용
	if( GetOwnerRole() == ROLE_Authority )
		_Server_CreateAuthorityDetector( InDetectEffectInfo );
}

/**
 *  FR4SkillDetectInfo 멤버를 찾아서 키를 부여.
 */
void UR4SkillBase::_Server_ParseSkillDetectInfo()
{
	if ( !ensureMsgf( GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server") ) )
		return;

	for ( const auto& [prop, value] : TPropertyValueRange<FStructProperty>( GetClass(), this ) )
	{
		// FR4SkillDetectInfo 타입의 struct를 찾아서 Key값을 부여.
		if ( prop->Struct != FR4SkillDetectInfo::StaticStruct() )
			continue;

		const FR4SkillDetectInfo* c_skillDetectPtr = static_cast<const FR4SkillDetectInfo*>(value);
		FR4SkillDetectInfo* skillDetectPtr = const_cast<FR4SkillDetectInfo*>(c_skillDetectPtr);

		// Server의 경우 FR4SkillDetectInfo에 Key 값을 부여.
		skillDetectPtr->DetectorServerKey = ++CachedSkillDetectInfoCount;
	}
}

/**
 *  Collision 설정을 변경하지 않는 Dummy Detector 생성.
 *  @param InSkillDetectInfo : Detect 정보
 */
void UR4SkillBase::_CreateDummyDetector( const FR4SkillDetectInfo& InSkillDetectInfo )
{
	if( InSkillDetectInfo.DetectorServerKey == Skill::G_InvalidDetectorKey )
	{
		LOG_WARN( R4Skill, TEXT("Detector Key Is Invalid. Check Replicate State."));
		return;
	}

	// 이미 Server Side에서 생성이 된 Detector이면 Dummy를 생성하지 않음
	if(int32 removeCnt = Client_CachedServerDetector.Remove( InSkillDetectInfo.DetectorServerKey );
		removeCnt > 0)
	{
		return;
	}
	
	// Dummy용 Detector 생성
	TScriptInterface<IR4DetectorInterface> detector(OBJECT_POOL->GetObject(InSkillDetectInfo.DetectClass));
	if(!IsValid(detector.GetObject()) || detector.GetInterface() == nullptr)
	{
		OBJECT_POOL->ReturnPoolObject(detector.GetObject());
		return;
	}

	// origin 설정
	FTransform origin;
	if ( IsValid(GetOwner()) )
	{
		origin = GetOwner()->GetActorTransform();

		// Attach to mesh?
		if( InSkillDetectInfo.bAttachToMesh )
		{
			ACharacter* parentActor = Cast<ACharacter>( GetOwner() );
			USkeletalMeshComponent* parentSkel = parentActor ? parentActor->GetMesh() : nullptr;

			origin = _AttachDetectorToTargetMesh( Cast<AActor>( detector.GetObject() ), parentSkel, InSkillDetectInfo.MeshSocketName );
		}
	}

	// Dummy의 경우 Setup만, Execute Detect는 실제로 실행하지 않음.
	detector->SetupDetect( origin, InSkillDetectInfo.DetectDesc );
	
	// Dummy에 Push
	Client_CachedDetectorDummy.Emplace( InSkillDetectInfo.DetectorServerKey, detector.GetObject() );
}

/**
 *  Collision을 활성화 시킨 Detector 생성. 
 *  @param InDetectEffectInfo : 탐지 정보 및 효과
 */
void UR4SkillBase::_Server_CreateAuthorityDetector( const FR4DetectEffectWrapper& InDetectEffectInfo )
{
	if ( !ensureMsgf( GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server") ) )
		return;
	
	if( InDetectEffectInfo.DetectInfo.DetectorServerKey == Skill::G_InvalidDetectorKey )
	{
		LOG_WARN( R4Skill, TEXT("Detector Key Is Invalid. Check Replicate State."));
		return;
	}
	
	// Detector 준비
	TScriptInterface<IR4DetectorInterface> detector(OBJECT_POOL->GetObject(InDetectEffectInfo.DetectInfo.DetectClass));
	if(!IsValid(detector.GetObject()) || detector.GetInterface() == nullptr)
	{
		OBJECT_POOL->ReturnPoolObject(detector.GetObject());
		return;
	}

	// this capture, weak lambda 사용
	// bind OnBeginDetect buff, damage
	detector.GetInterface()->OnBeginDetect().AddWeakLambda(this,
		[this, &InDetectEffectInfo](const FR4DetectResult& InDetectResult)
	{
		Server_ApplyBuffs( InDetectResult.DetectedActor.Get(), InDetectEffectInfo.EffectInfo.OnBeginDetectBuffs );
		Server_ApplyDamages( InDetectResult.DetectedActor.Get(), InDetectEffectInfo.EffectInfo.OnBeginDetectDamages );
	});
	
	// bind OnEndDetect buff, damage
	detector.GetInterface()->OnEndDetect().AddWeakLambda(this,
[this, &InDetectEffectInfo](const FR4DetectResult& InDetectResult)
	{
		Server_ApplyBuffs( InDetectResult.DetectedActor.Get(), InDetectEffectInfo.EffectInfo.OnEndDetectBuffs );
		Server_ApplyDamages( InDetectResult.DetectedActor.Get(), InDetectEffectInfo.EffectInfo.OnEndDetectDamages );
	});
	
	// origin 설정
	FTransform origin;
	if ( IsValid(GetOwner()) )
	{
		origin = GetOwner()->GetActorTransform();

		// Attach to mesh?
		if( InDetectEffectInfo.DetectInfo.bAttachToMesh )
		{
			ACharacter* parentActor = Cast<ACharacter>( GetOwner() );
			USkeletalMeshComponent* parentSkel = parentActor ? parentActor->GetMesh() : nullptr;

			origin = _AttachDetectorToTargetMesh( Cast<AActor>( detector.GetObject() ), parentSkel, InDetectEffectInfo.DetectInfo.MeshSocketName );
		}
	}
	
	// Setup & detect 실행
	detector.GetInterface()->SetupDetect( origin, InDetectEffectInfo.DetectInfo.DetectDesc );
	detector.GetInterface()->ExecuteDetect();

	// Dummy가 필요한 Detector였다면, Owner Client에게 Dummy 제거 명령 전송
	if ( InDetectEffectInfo.DetectInfo.bHasVisual )
		_ClientRPC_RemoveDummy( InDetectEffectInfo.DetectInfo.DetectorServerKey );
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
		LOG_WARN( R4Skill, TEXT("InDetector or InTargetMesh is invalid"));
		return FTransform::Identity;
	}
	
	// parent skeleton에 부착
	InDetector->AttachToComponent( InTargetMesh, FAttachmentTransformRules::KeepWorldTransform, InSocketName );
	return InTargetMesh->GetSocketTransform( InSocketName );
}

/**
 *  Server -> Owner Client로 Dummy 제거 요청.
 *  @param InDetectorKey : Server에서 부여한 Detector Key
 */
void UR4SkillBase::_ClientRPC_RemoveDummy_Implementation( uint32 InDetectorKey )
{
	if(GetOwnerRole() != ROLE_AutonomousProxy)
		return;

	// InDetectorKey에 맞는 Server Detector는 생성했으니 이만 제거
	if(auto it = Client_CachedDetectorDummy.Find( InDetectorKey ))
	{
		if(it->IsValid())
			OBJECT_POOL->ReturnPoolObject(it->Get());
		
		Client_CachedDetectorDummy.Remove( InDetectorKey );
		return;
	}

	// 아직 Dummy 생성이 안된 상태이면 Dummy를 생성하지 않도록 캐싱
	Client_CachedServerDetector.Emplace( InDetectorKey );
}