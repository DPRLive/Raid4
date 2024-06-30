// Fill out your copyright notice in the Description page of Project Settings.


#include "R4SkillBase.h"
#include "../Handler/CoolTimeHandler.h"
#include "../Interface/R4Detectable.h"

#include <GameFramework/Character.h>
#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4SkillBase)

UR4SkillBase::UR4SkillBase()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);

	CachedLastActivateTime = 0.f;
}

void UR4SkillBase::BeginPlay()
{
	Super::BeginPlay();

	CoolTimeHandler = MakeUnique<FCoolTimeHandler>();

	// bind anim affect index <-> DetectNotify
	for(const auto& [prop, value] : TPropertyValueRange<FStructProperty>(GetClass(), this))
	{
		// FSkillAnimInfo 타입의 struct를 찾아서 bind
		if(prop->Struct == FSkillAnimInfo::StaticStruct())
		{
			FSkillAnimInfo* animInfo = prop->ContainerPtrToValuePtr<FSkillAnimInfo>(this);
			UAnimMontage* anim = animInfo->SkillAnim;
			if(!IsValid(anim))
				return;
			
			for(auto& [notifyIdx, affect] : animInfo->DetectNotify)
			{
				if(anim->Notifies.IsValidIndex(notifyIdx))
					BindAffect(anim->Notifies[notifyIdx].Notify, affect);
			}
		}
	}
}

#if WITH_EDITOR
void UR4SkillBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.MemberProperty == nullptr)
		return;
	
	// 변경된 프로퍼티가 FSkillAnimInfo 형식이면, 해당 Anim에서 Notify를 읽어와 배열을 자동으로 채움
	if(FStructProperty* prop = CastField<FStructProperty>(PropertyChangedEvent.MemberProperty);
		prop != nullptr &&
		prop->Struct == FSkillAnimInfo::StaticStruct())
	{
		FSkillAnimInfo* animInfo = prop->ContainerPtrToValuePtr<FSkillAnimInfo>(this);
		UAnimMontage* anim = animInfo->SkillAnim;
		if(!IsValid(anim))
			return;

		// Detect Notify의 index들을 찾아냄 
		TSet<int32> idxs;
		for(int32 i = 0; i < anim->Notifies.Num(); i++)
		{
			if(IR4Detectable* detectNotify = Cast<IR4Detectable>(anim->Notifies[i].Notify))
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
 *  스킬이 사용 가능 상태인지 판단한다.
 */
bool UR4SkillBase::CanUseSkill()
{
	return FMath::IsNearlyEqual(0.f, CoolTimeHandler->GetCoolTime());
}

/**
 *  AnimMontage를 Play한다.
 */
float UR4SkillBase::PlaySkillAnim(const FSkillAnimInfo& InSkillAnimInfo, float InPlayRate, const FName& InSectionName)
{
	if(ACharacter* owner = Cast<ACharacter>(GetOwner()))
	{
		return owner->PlayAnimMontage(InSkillAnimInfo.SkillAnim, InPlayRate, InSectionName);

		// TODO : hit check
	}

	return 0.f;
}

/**
 *  Anim Montage를 stop
 */
void UR4SkillBase::StopAllAnim()
{
	if(ACharacter* owner = Cast<ACharacter>(GetOwner()))
		owner->StopAnimMontage();

	// TODO : clear reserved hitcheck
}

/**
 *  Detectable과 Affect를 연결
 *  @param InDetectable : 탐지하는 주체 Object
 *  @param InAffectInfo : 입힐 영향
 */
void UR4SkillBase::BindAffect(UObject* InDetectable, const FString& InAffectInfo)
{
	if(!IsValid(InDetectable))
		return;

	if(IR4Detectable* detectableObj = Cast<IR4Detectable>(InDetectable))
	{
		auto affectLambda = [owner = TWeakObjectPtr<UR4SkillBase>(this), &affectInfo = InAffectInfo]
				(const FDetectResult& InDetectResult)
		{
			if(owner.IsValid())
				owner->ApplyAffect(InDetectResult, affectInfo);
		};
		
		// Detect 시작 Bind
		detectableObj->OnBeginDetect().AddLambda(affectLambda);

		// Detect 종료 Bind
		detectableObj->OnEndDetect().AddLambda(affectLambda);
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
