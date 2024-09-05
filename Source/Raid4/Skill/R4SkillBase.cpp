// Fill out your copyright notice in the Description page of Project Settings.

#include "R4SkillBase.h"
#include "../Buff/R4BuffReceiveInterface.h"
#include "../Damage/R4DamageReceiveInterface.h"
#include "../Detect/R4NotifyDetectInterface.h"
#include "../Detect/Detector/R4DetectorInterface.h"
#include "../Detect/DetectResult.h"
#include "../Util/UtilDamage.h"

#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>

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
			[this, &InDetectEffectInfo]()
			{
				// 탐지 실행
				_ExecuteDetect(InDetectEffectInfo);
			});
	}
}

/**
 *  Detect 실행
 *  @param InDetectEffectInfo : 탐지 클래스 및 효과.
 */
void UR4SkillBase::_ExecuteDetect( const FR4DetectEffectWrapper& InDetectEffectInfo )
{
	if(!IsValid(InDetectEffectInfo.DetectInfo.DetectClass))
		return;

	// Ready Detect
	UObject* detectObj = OBJECT_POOL->GetObject(InDetectEffectInfo.DetectInfo.DetectClass);

	if(!IsValid(detectObj))
		return;
	
	if( IR4DetectorInterface* detectInterface = Cast<IR4DetectorInterface>(detectObj) )
	{
		// this capture, weak lambda 사용

		// bind OnBeginDetect buff, damage
		detectInterface->OnBeginDetect().AddWeakLambda(this,
			[this, &InDetectEffectInfo](const FDetectResult& InDetectResult)
		{
			_ApplyBuffs( InDetectResult, InDetectEffectInfo.EffectInfo.OnBeginDetectBuffs );
			_ApplyDamages( InDetectResult, InDetectEffectInfo.EffectInfo.OnBeginDetectDamages );
		});

		// bind OnEndDetect buff, damage
		detectInterface->OnEndDetect().AddWeakLambda(this,
	[this, &InDetectEffectInfo](const FDetectResult& InDetectResult)
		{
			_ApplyBuffs( InDetectResult, InDetectEffectInfo.EffectInfo.OnEndDetectBuffs );
			_ApplyDamages( InDetectResult, InDetectEffectInfo.EffectInfo.OnEndDetectDamages );
		});
	}

	// TODO : Execute Detect ...
}

/**
 *  Detect 시 특정한 버프들을 적용.
 *  @param InDetectResult : 탐지 결과.
 *  @param InSkillBuffInfos : 입힐 SkillBuff 들
 */
void UR4SkillBase::_ApplyBuffs( const FDetectResult& InDetectResult, const TArray<FR4SkillBuffInfo>& InSkillBuffInfos ) const
{
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
void UR4SkillBase::_ApplyDamages(const FDetectResult& InDetectResult, const TArray<FR4SkillDamageInfo>& InSkillDamageInfos) const
{
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

/**
 *  스킬 사용의 유효성을 검증한다.
 *  @param InActivateTime : 클라이언트가 스킬 사용 당시의 서버 시간
 */
// bool UR4SkillBase::ServerRPC_ActivateSkill_Validate(const TSoftObjectPtr<UAnimMontage>& InSkillAnim, float InActivateTime)
// {
// 	// 사용한 시간이 쿨타임 + 오차허용시간 보다 짧다?
// 	// if((InActivateTime - LastActivateTime) <  (/* 쿹타임 - */ Validation::G_AcceptMinCoolTime) )
// 	// 	return false;
// 	return true;
// }
//
// /**
//  *  서버로 스킬 사용을 알린다.
//  *  @param InActivateTime : 클라이언트에서 스킬을 사용했을 때의 서버 시간
//  */
// void UR4SkillBase::ServerRPC_ActivateSkill_Implementation(const TSoftObjectPtr<UAnimMontage>& InSkillAnim, float InActivateTime)
// {
// 	// 스킬 사용시간 기록
// 	CachedLastActivateTime = InActivateTime;
// }
