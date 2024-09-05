#pragma once

#include "../Damage/R4DamageStruct.h"
#include "../Buff/R4BuffStruct.h"

#include "R4SkillStruct.generated.h"

class UR4BuffBase;
class UAnimMontage;

/**
 * Skill에서 진행할 탐지에 관한 정보
 */
USTRUCT()
struct FR4SkillDetectInfo
{
	GENERATED_BODY()

	// TODO : Detect Actor Class.. //
	// 어떤 방식으로 Detect를 진행할 것인지.
	UPROPERTY( EditAnywhere )
	TSubclassOf<AActor> DetectClass;

	// TODO : Pos, Rot, ...
};

/**
 * Skill이 줄 버프 대한 정보.
 */
USTRUCT()
struct FR4SkillBuffInfo
{
	GENERATED_BODY()

	// 버프를 적용할 타겟. Instigator : 나. Victim : Detect 된 Actor
	UPROPERTY( EditAnywhere )
	ETargetType Target;
	
	// 적용할 버프 클래스
	UPROPERTY( EditAnywhere )
	TSubclassOf<UR4BuffBase> BuffClass;

	// 적용할 버프의 세팅
	UPROPERTY( EditAnywhere )
	FR4BuffSettingDesc BuffSetting;
};

/**
 * Skill이 줄 데미지에 대한 정보.
 */
USTRUCT()
struct FR4SkillDamageInfo
{
	GENERATED_BODY()

	// 데미지를 적용할 타겟. Instigator : 나. Victim : Detect 된 Actor
	UPROPERTY( EditAnywhere )
	ETargetType Target;
	
	// 적용할 데미지 정보
	UPROPERTY( EditAnywhere )
	FR4DamageApplyDesc DamageInfo;
};

/**
 * Skill에서 줄 영향에 대한 정보
 */
USTRUCT()
struct FR4SkillEffectInfo
{
	GENERATED_BODY()
	
	// OnDetect 시 적용할 버프
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillBuffInfo> OnBeginDetectBuffs;
	
	// OnEndDetect 시 적용할 버프
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillBuffInfo> OnEndDetectBuffs;

	// OnDetect 시 적용할 데미지
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillDamageInfo> OnBeginDetectDamages;
	
	// OnEndDetect 시 적용할 데미지
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillDamageInfo> OnEndDetectDamages;
};

/**
 * Skill에서 무언가 탐지하고 줄 영향을 래핑
 */
USTRUCT()
struct FR4DetectEffectWrapper
{
	GENERATED_BODY()

	// 탐지할 방법
	UPROPERTY( EditAnywhere )
	FR4SkillDetectInfo DetectInfo;

	// 탐지 시 줄 영향
	UPROPERTY( EditAnywhere )
	FR4SkillEffectInfo EffectInfo;
};

/**
 * Skill을 위한 Animation의 정보.
 * Animation과 Animation 작동 시 필요한 히트 체크 정보를 제공
 */
USTRUCT()
struct FR4SkillAnimInfo
{
	GENERATED_BODY()
	
	FR4SkillAnimInfo()
		: SkillAnim(nullptr)
	{}
	
	// 발동할 Skill Anim
	UPROPERTY( EditAnywhere )
	TObjectPtr<UAnimMontage> SkillAnim;

	// Anim의 각 Notify와, Notify 번호에 맞는 무언가 탐지하고 줄 영향 지정
	// {Notify index ( AnimMontage에서 몇번째 Notify인지 ), 탐지 및 효과 정보} 
	UPROPERTY( EditAnywhere )
	TMap<int32, FR4DetectEffectWrapper> DetectNotify;
};