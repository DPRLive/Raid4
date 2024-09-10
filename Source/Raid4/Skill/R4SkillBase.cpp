// Fill out your copyright notice in the Description page of Project Settings.

#include "R4SkillBase.h"
#include "../Buff/R4BuffReceiveInterface.h"
#include "../Damage/R4DamageReceiveInterface.h"
#include "../Detect/R4NotifyDetectInterface.h"
#include "../Detect/Detector/R4DetectorInterface.h"
#include "../Detect/R4DetectStruct.h"
#include "../Util/UtilDamage.h"

#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>
#include <GameFramework/Pawn.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4SkillBase)

UR4SkillBase::UR4SkillBase()
{
	// 필요 시에만 Ticking
    PrimaryComponentTick.bCanEverTick = false;
	
    SetIsReplicatedByDefault(true);
}

void UR4SkillBase::BeginPlay()
{
	Super::BeginPlay();

	CoolTimeChecker = MakeUnique<TTimeLimitChecker<int32>>();

	// bind DetectNotify <-> FR4SkillDetectEffectInfo
	// Detect는 Local에서 직접 진행.
	for(const auto& [prop, value] : TPropertyValueRange<FStructProperty>(GetClass(), this))
	{
		// FSkillAnimInfo 타입의 struct를 찾아서 bind
		if(prop->Struct != FR4SkillAnimInfo::StaticStruct())
			continue;
		
		FR4SkillAnimInfo* animInfo = prop->ContainerPtrToValuePtr<FR4SkillAnimInfo>(this);
		UAnimMontage* anim = animInfo->SkillAnim;
		if(!IsValid(anim))
			return;
		
		for(const auto& [notifyIdx, detectEffectInfo] : animInfo->DetectNotify)
		{
			if(!anim->Notifies.IsValidIndex(notifyIdx))
				continue;
	
			_BindDetectAndEffect(anim->Notifies[notifyIdx].Notify, detectEffectInfo);
		}
	}
}

#if WITH_EDITOR
void UR4SkillBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.MemberProperty == nullptr)
		return;
	
	// 변경된 프로퍼티가 FSkillAnimInfo 형식이면, 해당 Anim에서 Notify를 읽어와 배열을 자동으로 채움. 하하 아주 편리하지?
	if(FStructProperty* prop = CastField<FStructProperty>(PropertyChangedEvent.MemberProperty);
		prop != nullptr &&
		prop->Struct == FR4SkillAnimInfo::StaticStruct())
	{
		FR4SkillAnimInfo* animInfo = prop->ContainerPtrToValuePtr<FR4SkillAnimInfo>(this);
		UAnimMontage* anim = animInfo->SkillAnim;
		
		if(!IsValid(anim))
			return;

		// Detect Notify의 index들을 찾아냄 
		TSet<int32> idxs;
		for(int32 i = 0; i < anim->Notifies.Num(); i++)
		{
			if(IR4NotifyDetectInterface* detectNotify = Cast<IR4NotifyDetectInterface>(anim->Notifies[i].Notify))
				idxs.Emplace(i);
		}

		// 필요 없는 index는 제거
		for(auto it = animInfo->DetectNotify.CreateIterator(); it; ++it)
		{
			if(idxs.Find(it->Key) == nullptr)
			{
				it.RemoveCurrent();
				continue;
			}
			idxs.Remove(it->Key);
		}

		// 기존에 없는 index는 추가
		for(const auto& idx : idxs)
		{
			if(animInfo->DetectNotify.Find(idx) == nullptr)
				animInfo->DetectNotify.Emplace(idx);
		}

		// 보기 편하게 sort
		animInfo->DetectNotify.KeySort([](const int32& idx1, const int32& idx2)
		{
			return idx1 < idx2;
		});
	}
}
#endif

/**
 *  스킬 사용이 가능한지 판단
 *  TODO : 현재 스킬 사용중인지 판단 추가
 */
bool UR4SkillBase::CanActivateSkill()
{
	bool bReady = true;

	// 스킬 자체에 대한 쿨타임 체크
	// if(CoolTimeChecker.IsValid())
	// {
	// 	AActor* owner = GetOwner();
	// 	
	// 	// 서버에서 Remote를 체크하는 경우 약간의 오차 허용
	// 	if(	IsValid(owner)
	// 		&& owner->GetLocalRole() == ROLE_Authority
	// 		&& owner->GetRemoteRole() == ROLE_AutonomousProxy )
	// 	{
	// 		bReady &= CoolTimeChecker->IsTimeLimitExpired(static_cast<int32>(ER4SkillTimeType::Skill), R4GetServerTimeSeconds(GetWorld()) + Validation::G_AcceptMinTime);
	// 	}			
	// 	else
	// 		bReady &= CoolTimeChecker->IsTimeLimitExpired(static_cast<int32>(ER4SkillTimeType::Skill), R4GetServerTimeSeconds(GetWorld()));
	// }

	return bReady;
}

/**
 *  DetectNotify <-> FR4DetectEffectWrapper 연결
 *  @param InDetectNotify : 탐지 타이밍 알림을 전달할 객체
 *  @param InDetectEffectInfo : Skill에서 무언가 탐지하고 줄 영향
 */
void UR4SkillBase::_BindDetectAndEffect( const TScriptInterface<IR4NotifyDetectInterface>& InDetectNotify, const FR4DetectEffectWrapper& InDetectEffectInfo )
{
	if( IR4NotifyDetectInterface* detectNotifyObj = InDetectNotify.GetInterface() )
	{
		// this 캡처, WeakLambda 사용
		detectNotifyObj->OnNotifyDetect( GetOwner() ).BindWeakLambda(this,
			[this, &InDetectEffectInfo, reqObj = InDetectNotify.GetObject() ]()
			{
				// 탐지 실행
				_ExecuteDetect( reqObj, InDetectEffectInfo );
			});
	}
}
 
/**
 *  Detect 실행
 *  Owner Client(Disable Collision), Server(+ Enable Collision) 경우에만 생성
 *  Owner Client : Visual 적인 요소가 필요한 경우 Dummy 생성, 후에 서버에서 생성되면 Dummy 제거
 *  Server : Collision을 포함한 실제 Detector 생성
 *  @param InRequestObj : 탐지 시점을 알린 객체. (AnimNotify 라던가)
 *  @param InDetectEffectInfo : 탐지 클래스 및 효과.
 */
void UR4SkillBase::_ExecuteDetect( const UObject* InRequestObj, const FR4DetectEffectWrapper& InDetectEffectInfo )
{
	if(!IsValid(InDetectEffectInfo.DetectInfo.DetectClass) || !IsValid(InRequestObj))
		return;

	// Simulated Proxy인 경우, 필요 없음.
	if(GetOwnerRole() == ROLE_SimulatedProxy)
		return;

	// Owner Client 이지만 Dummy가 필요한 경우 (Visual 적인 요소가 필요한 경우)
	if(GetOwnerRole() == ROLE_AutonomousProxy && InDetectEffectInfo.DetectInfo.bHasVisual)
	{
		_CreateDummyDetector( InRequestObj, InDetectEffectInfo.DetectInfo );
		return;
	}

	// Server인 경우, 실제 Collision을 사용 & Effect 적용
	if(GetOwnerRole() == ROLE_Authority)
		_CreateAuthorityDetector( InRequestObj, InDetectEffectInfo );
}

/**
 *  Collision 설정을 변경하지 않는 Dummy Detector 생성.
 *  @param InRequestObj : Detect를 요청한 객체. Authority에 의한 Dummy Detector 제거 등에 사용
 *  @param InSkillDetectInfo : 탐지 정보
 */
void UR4SkillBase::_CreateDummyDetector( const UObject* InRequestObj, const FR4SkillDetectInfo& InSkillDetectInfo )
{
	// Detector 준비 Dummy를 생성.
	TScriptInterface<IR4DetectorInterface> detector(OBJECT_POOL->GetObject(InSkillDetectInfo.DetectClass));
	if(!IsValid(detector.GetObject()) || detector.GetInterface() == nullptr)
	{
		OBJECT_POOL->ReturnPoolObject(detector.GetObject());
		return;
	}

	// TODO : Origin 넘기는 방법 여러가지로 ..
	FTransform origin;
	if(GetOwner())
		origin = GetOwner()->GetActorTransform();
	
	detector->ExecuteDetect( origin, InSkillDetectInfo.DetectDesc);

	// Dummy에 Push
	CachedDetectorDummy.Emplace( InRequestObj, detector.GetObject() );
}

/**
 *  Collision을 활성화 시킨 Detector 생성. 
 *  @param InRequestObj : Detect를 요청한 객체. Authority에 의한 Dummy Detector 제거 등에 사용
 *  @param InDetectEffectInfo : 탐지 정보 및 효과
 */
void UR4SkillBase::_CreateAuthorityDetector( const UObject* InRequestObj, const FR4DetectEffectWrapper& InDetectEffectInfo )
{
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
		_Server_ApplyBuffs( InDetectResult, InDetectEffectInfo.EffectInfo.OnBeginDetectBuffs );
		_Server_ApplyDamages( InDetectResult, InDetectEffectInfo.EffectInfo.OnBeginDetectDamages );
	});
	
	// bind OnEndDetect buff, damage
	detector.GetInterface()->OnEndDetect().AddWeakLambda(this,
[this, &InDetectEffectInfo](const FR4DetectResult& InDetectResult)
	{
		_Server_ApplyBuffs( InDetectResult, InDetectEffectInfo.EffectInfo.OnEndDetectBuffs );
		_Server_ApplyDamages( InDetectResult, InDetectEffectInfo.EffectInfo.OnEndDetectDamages );
	});

	// enable collision
	if(AActor* detectActor = Cast<AActor>(detector.GetObject()))
		detectActor->SetActorEnableCollision(true);

	// TODO : Origin 넘기는 방법 여러가지로 ..
	FTransform origin;
	if(GetOwner())
		origin = GetOwner()->GetActorTransform();
	
	// 탐지 실행
	detector.GetInterface()->ExecuteDetect(origin, InDetectEffectInfo.DetectInfo.DetectDesc);

	// Owner Client에게 Dummy 제거 명령 전송
	_ClientRPC_RemoveDummy( InRequestObj );
}

/**
 *  Collision을 활성화 시킨 Detector 생성. 
 *  @param InRequestObj : Detect를 요청한 객체. Authority에 의한 Dummy Detector 제거 등에 사용
 */
void UR4SkillBase::_ClientRPC_RemoveDummy_Implementation( const UObject* InRequestObj )
{
	if(GetOwnerRole() != ROLE_AutonomousProxy)
		return;
	
	// InRequestObj가 요청했던 Detector는 생성했으니 이만 제거
	for(auto it = CachedDetectorDummy.CreateIterator(); it; ++it)
	{
		// Key 또는 Value가 Invalid한 객체에 대한 element는 삭제
		if(!(it->Key.IsValid()) || !(it->Value.IsValid()))
		{
			it.RemoveCurrentSwap();
			continue;
		}

		// match 되는 key를 찾으면 dummy 제거.
		if(it->Key.Get() == InRequestObj)
		{
			OBJECT_POOL->ReturnPoolObject(it->Value.Get());
			it.RemoveCurrentSwap();

			return;
		}
	}
}

/**
 *  Detect 시 특정한 버프들을 적용.
 *  @param InDetectResult : 탐지 결과.
 *  @param InSkillBuffInfos : 입힐 SkillBuff 들
 */
void UR4SkillBase::_Server_ApplyBuffs( const FR4DetectResult& InDetectResult, const TArray<FR4SkillBuffInfo>& InSkillBuffInfos ) const
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
			target = InDetectResult.DetectedActor.Get();

		// 버프 적용
		if( IR4BuffReceiveInterface* victim = Cast<IR4BuffReceiveInterface>(target) )
			victim->ReceiveBuff(GetOwner(), buffInfo.BuffClass, buffInfo.BuffSetting);
	}
}

/**
 *  Detect 시 특정한 데미지들을 적용.
 *  @param InDetectResult : 탐지 결과.
 *  @param InSkillDamageInfos : 입힐 데미지들.
 */
void UR4SkillBase::_Server_ApplyDamages(const FR4DetectResult& InDetectResult, const TArray<FR4SkillDamageInfo>& InSkillDamageInfos) const
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
			target = InDetectResult.DetectedActor.Get();

		// 데미지 적용
		if( IR4DamageReceiveInterface* victim = Cast<IR4DamageReceiveInterface>(target) )
		{
			FR4DamageReceiveInfo damageRecvInfo = UtilDamage::CalculateDamageReceiveInfo( GetOwner(), target, damageInfo.DamageInfo );
			victim->ReceiveDamage( GetOwner(), damageRecvInfo );
		}
	}
}