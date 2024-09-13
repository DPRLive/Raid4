#pragma once

#include "../Detect/R4DetectStruct.h"
#include "../Buff/R4BuffStruct.h"
#include "../Damage/R4DamageStruct.h"

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

	FR4SkillDetectInfo()
	: DetectClass( nullptr )
	, bHasVisual( false )
	, DetectDesc( FR4DetectDesc() )
	, DetectorServerKey( Skill::G_InvalidDetectorKey )
	{}
	
	// 어떤 클래스로 Detect를 진행할 것인지.
	// Collision이 Disable된 상태로 설정.
	UPROPERTY( NotReplicated, EditAnywhere, meta = ( MustImplement = "R4DetectorInterface" ) )
	TSubclassOf<AActor> DetectClass;
	
	// 해당 Detect Class가 Visual적인 요소를 포함하고 있는지?
	// ex) 투사체
	UPROPERTY( NotReplicated, EditAnywhere )
	uint8 bHasVisual:1;
	
	// 탐지에 관한 Parameter.
	UPROPERTY( NotReplicated, EditAnywhere )
	FR4DetectDesc DetectDesc;

	// Server와 Client 사이 Detector를 구분하기 위한 Key값.
	UPROPERTY( Transient, VisibleInstanceOnly )
	uint32 DetectorServerKey;
};

/**
 * Skill이 줄 버프 대한 정보.
 */
USTRUCT()
struct FR4SkillBuffInfo
{
	GENERATED_BODY()

	FR4SkillBuffInfo()
	: Target( ETargetType::Victim )
	, BuffClass( nullptr )
	, BuffSetting( FR4BuffSettingDesc() )
	{}
	
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

	FR4SkillDamageInfo()
	: Target( ETargetType::Victim )
	, DamageInfo( FR4DamageApplyDesc() )
	{}
	
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
	UPROPERTY( EditAnywhere, Category = OnBegin )
	TArray<FR4SkillBuffInfo> OnBeginDetectBuffs;
	
	// OnEndDetect 시 적용할 버프
	UPROPERTY( EditAnywhere, Category = OnEnd )
	TArray<FR4SkillBuffInfo> OnEndDetectBuffs;

	// OnDetect 시 적용할 데미지
	UPROPERTY( EditAnywhere, Category = OnBegin )
	TArray<FR4SkillDamageInfo> OnBeginDetectDamages;
	
	// OnEndDetect 시 적용할 데미지
	UPROPERTY( EditAnywhere, Category = OnEnd )
	TArray<FR4SkillDamageInfo> OnEndDetectDamages;
};

/**
 * Skill에서 무언가 탐지하고 줄 영향을 래핑
 */
USTRUCT()
struct FR4DetectEffectWrapper
{
	GENERATED_BODY()

	FR4DetectEffectWrapper()
	: DetectInfo( FR4SkillDetectInfo() )
	, EffectInfo( FR4SkillEffectInfo() )
	{}
	
	// 탐지할 방법
	UPROPERTY( EditAnywhere )
	FR4SkillDetectInfo DetectInfo;

	// 탐지 시 줄 영향
	UPROPERTY( EditAnywhere )
	FR4SkillEffectInfo EffectInfo;
};

/**
 * Skill을 위한 Animation의 정보.
 * Animation과 Animation 작동 시 필요한 히트 체크 정보를 제공.
 * Skill Anim key를 서버에서 받기 위해 Replicate로 설정.
 */
USTRUCT()
struct FR4SkillAnimInfo
{
	GENERATED_BODY()
	
	FR4SkillAnimInfo()
	: SkillAnim( nullptr )
	, SkillAnimServerKey( Skill::G_InvalidSkillAnimKey )
	{}
	
	// 발동할 Skill Anim
	UPROPERTY( NotReplicated, EditAnywhere )
	TObjectPtr<UAnimMontage> SkillAnim;

	// Anim의 각 Notify와, Notify 번호에 맞는 무언가 탐지하고 줄 영향 지정
	// {Notify index ( AnimMontage에서 몇번째 Notify인지 ), 탐지 및 효과 정보} 
	UPROPERTY( NotReplicated, EditAnywhere )
	TMap<int32, FR4DetectEffectWrapper> DetectNotify;

	// Server와 Client 사이 Skill Anim을 구분하기 위한 Key값.
	UPROPERTY( Transient, VisibleInstanceOnly )
	uint32 SkillAnimServerKey;
};