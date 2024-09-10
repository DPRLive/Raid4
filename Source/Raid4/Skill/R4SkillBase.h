// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4SkillStruct.h"
#include "../Handler/TimeLimitChecker.h"

#include <Components/ActorComponent.h>

#include "R4SkillBase.generated.h"

struct FR4DetectResult;
class IR4NotifyDetectInterface;

/**
 * Skill에서 사용되는 '시간'들에 대한 타입.
 * (스킬 쿨타임, anim section switch간 cooltime등.)
 */
// UENUM()
// enum class ER4SkillTimeType : uint8
// {
// 	Skill,		// 스킬 자체에 걸리는 쿨타임
// 	// Section,	// Anim Montage의 섹션간 걸리는 쿨타임
// 	Max
// };

/**
 * Skill의 Base가 되는 클래스.
 * 스킬을 위한 기능들을 제공
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
	// 스킬 사용이 가능한지 판단
	virtual bool CanActivateSkill();
	
	//.. valid 체크 시
 	// Ownership이 있는 캐릭터인지 check는 필요x, Server RPC 자체가 ownership이 있어야만 보낼 수 있음

private:
	// DetectNotify <-> FR4DetectEffectWrapper 연결
	void _BindDetectAndEffect( const TScriptInterface<IR4NotifyDetectInterface>& InDetectNotify, const FR4DetectEffectWrapper& InDetectEffectInfo );
	
	// 특정 SkillAnim을 Play
	//void _PlaySkillAnim(const FR4SkillAnimInfo& InSkillAnimInfo);
	
	// Detect 실행
	void _ExecuteDetect( const UObject* InRequestObj, const FR4DetectEffectWrapper& InDetectEffectInfo );

	// Dummy Detector 생성
	void _CreateDummyDetector( const UObject* InRequestObj, const FR4SkillDetectInfo& InSkillDetectInfo );
	
	// Authority Detector 생성
	void _CreateAuthorityDetector( const UObject* InRequestObj, const FR4DetectEffectWrapper& InDetectEffectInfo );

	// Server -> All Client로 Dummy 제거 요청.
	UFUNCTION( Client, Unreliable )
	void _ClientRPC_RemoveDummy( const UObject* InRequestObj );
	
	// Detect 시 특정한 버프들을 적용.
	void _Server_ApplyBuffs( const FR4DetectResult& InDetectResult, const TArray<FR4SkillBuffInfo>& InSkillBuffInfos ) const;

	// Detect 시 특정한 데미지들을 적용.
	void _Server_ApplyDamages( const FR4DetectResult& InDetectResult, const TArray<FR4SkillDamageInfo>& InSkillDamageInfos ) const;

protected:
	// 쿹타임 체크를 위한 CoolTimeChecker
	TUniquePtr<TTimeLimitChecker<int32>> CoolTimeChecker;

	// Detector의 Dummy 체크를 위한 Array
	// {해당 Detector를 요청한 UObject, Detector}
	TArray<TPair<TWeakObjectPtr<const UObject>, TWeakObjectPtr<UObject>>> CachedDetectorDummy;
};