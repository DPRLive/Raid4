// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4SkillBase.generated.h"

struct FDetectResult;
class IR4DetectableInterface;
class FTimerHandler;
class UAnimMontage;

/**
 * Skill을 위한 Animation의 정보.
 * Animation과 특정 프레임 일 때 필요한 히트 체크 정보를 제공
 */
USTRUCT()
struct FSkillAnimInfo
{
	GENERATED_BODY()
	
	FSkillAnimInfo()
		: SkillAnim(nullptr)
		{}
	
	// 발동할 Skill Anim
	UPROPERTY( EditAnywhere )
	TObjectPtr<UAnimMontage> SkillAnim;

	// 레벨에서 무언가 탐지하는 Notify에 대한 행동 정의
	// {Notify index ( AnimMontage에서 몇번째 Notify인지), 행동} 
	// TODO : 행동을 정의 해야함
	UPROPERTY( EditAnywhere )
	TMap<int32, FString> DetectNotify;
};

/**
 * Skill의 Base가 되는 클래스.
 * 스킬을 위한 기본 기능들을 제공
 */
UCLASS( Abstract, HideCategories = (ComponentTick, Tags, Replication, ComponentReplication, Activation, Variable, Navigation, AssetUserData) )
class RAID4_API UR4SkillBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4SkillBase();

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	// 스킬이 사용 가능 상태인지 판단한다.
	virtual bool CanUseSkill();
	
protected:
	// Anim Montage를 Play한다.
	virtual float PlaySkillAnim(const FSkillAnimInfo& InSkillAnimInfo, float InPlayRate = 1.f, const FName& InSectionName = NAME_None);

	// Anim Montage를 stop
	virtual void StopAllAnim();

	// Affect를 입힌다
	virtual void ApplyAffect(const FDetectResult& InDetectResult, const FString& InAffect) { LOG_WARN(LogTemp, TEXT("Affect! %s"), *InAffect); }

	// Detectable과 Affect를 연결
	// TODO : Change TScriptInterface ? 
	void BindAffect(UObject* InDetectable, const FString& InString /* TODO : Affect*/);
	
protected:
	// 스킬 쿨타임을 위한 CoolTimeHandler
	TSharedPtr<FTimerHandler> CoolTimeHandler;
	
	// 마지막으로 발동한 시간 (서버)
	float CachedLastActivateTime;
};
