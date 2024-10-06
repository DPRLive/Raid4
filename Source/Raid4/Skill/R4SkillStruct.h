#pragma once

#include "../Detect/R4DetectStruct.h"
#include "../Buff/R4BuffStruct.h"

#include "R4SkillStruct.generated.h"

class UR4BuffBase;
class UAnimMontage;

/**
 * Skill에서 진행할 탐지에 관한 정보.
 */
USTRUCT()
struct FR4SkillDetectInfo
{
	GENERATED_BODY()

	FR4SkillDetectInfo()
	: DetectClass( nullptr )
	, DetectorNetFlag( 0 )
	, DetectorOriginCalculator( nullptr )
	, bAttachToMesh( false )
	, MeshSocketName( NAME_None )
	, DetectDesc( FR4DetectDesc() )
	{}
	
	// 어떤 클래스로 Detect를 진행할 것인지?
	// Dummy가 필요한 경우 (bHasVisual) Detect Class를 Replicate 설정해야함! 
	UPROPERTY( EditAnywhere, meta = ( MustImplement = "/Script/Raid4.R4DetectorInterface" ) )
	TSubclassOf<AActor> DetectClass;
	
	// Detector의 Network Spawn 정책을 설정
	// Replicated Detector일 시 Server에서만 Spawn 해야함.
	// particle이 있으나 투사체 등 (시각적인 요소 + 위치 같이) 중요하지 않다면 굳이 Replicate 하지 않고 Local 생성하는걸 권장
	UPROPERTY( EditAnywhere, meta = ( Bitmask, BitmaskEnum = "/Script/Raid4.ER4NetworkFlag" ) )
	uint8 DetectorNetFlag;

	// Detector의 Relative Location의 기준이 될 Origin을 계산하는 Class.
	UPROPERTY( EditAnywhere, meta = ( MustImplement = "/Script/Raid4.R4OriginCalculatorInterface" ) )
	TSubclassOf<UObject> DetectorOriginCalculator;
	
	// Detector를 Mesh에 Attach할 것인지?
	UPROPERTY( EditAnywhere )
	uint8 bAttachToMesh:1;

	// Skeletal Mesh Socket Name
	UPROPERTY( EditAnywhere, meta = ( EditCondition = "bAttachToMesh", EditConditionHides ) )
	FName MeshSocketName;
	
	// 탐지에 관한 Parameter.
	UPROPERTY( EditAnywhere )
	FR4DetectDesc DetectDesc;
};

/**
 * Skill이 Detect시 줄 버프 대한 정보. ( Server 에서만 적용 )
 * 데미지도 버프로 적용 시키면 됨
 */
USTRUCT()
struct FR4SkillDetectBuffInfo
{
	GENERATED_BODY()

	FR4SkillDetectBuffInfo()
	: Target( ETargetType::Victim )
	, BuffClass( nullptr )
	, BuffSetting ( FR4BuffSettingDesc() )
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
 * Skill Detector, Buff Wrapper
 */
USTRUCT()
struct FR4SkillDetectBuffWrapper
{
	GENERATED_BODY()

	// 사용할 Detect 정보
	UPROPERTY( EditAnywhere )
	FR4SkillDetectInfo DetectorInfo;
	
	// OnDetect 시 적용할 버프
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillDetectBuffInfo> Server_OnBeginDetectBuffs;
	
	// OnEndDetect 시 적용할 버프
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillDetectBuffInfo> Server_OnEndDetectBuffs;
};

/**
 * Skill에서 특정 시간에 Detect시 사용
 */
USTRUCT()
struct FR4SkillTimeDetectWrapper
{
	GENERATED_BODY()

	FR4SkillTimeDetectWrapper()
	: DelayTime( 0.f )
	, DetectEffect( FR4SkillDetectBuffWrapper() )
	{}

	// Delay Time, ( AnimMontage의 특정 Section Play된 후 시간 ) 
	UPROPERTY( EditAnywhere )
	float DelayTime;

	// 해당 시점에 할 Detect와 적용할 Buff
	UPROPERTY( EditAnywhere )
	FR4SkillDetectBuffWrapper DetectEffect;
};

/**
 * Skill에서 특정 시간에 Apply Buff시 사용
 */
USTRUCT()
struct FR4SkillTimeBuffWrapper
{
	GENERATED_BODY()

	FR4SkillTimeBuffWrapper()
	: DelayTime( 0.f )
	, BuffNetFlag ( 0 )
	, BuffClass( nullptr )
	, BuffSetting( FR4BuffSettingDesc() )
	{}
	
	// Delay Time
	UPROPERTY( EditAnywhere )
	float DelayTime;

	// Buff를 적용할 Network 정책을 설정
	UPROPERTY( EditAnywhere, meta = ( Bitmask, BitmaskEnum = "/Script/Raid4.ER4NetworkFlag" ) )
	uint8 BuffNetFlag;
	
	// 해당 Notify가 적용할 버프 클래스
	UPROPERTY( EditAnywhere )
	TSubclassOf<UR4BuffBase> BuffClass;

	// 해당 Notify가 버프의 세팅
	UPROPERTY( EditAnywhere )
	FR4BuffSettingDesc BuffSetting;
};

USTRUCT()
struct FR4SkillTimeDetectArray
{
	GENERATED_BODY()

	// 해당 시점에 할 Detect와 적용할 Buff
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillTimeDetectWrapper> DetectEffects;
};

USTRUCT()
struct FR4SkillTimeBuffArray
{
	GENERATED_BODY()

	// 해당 시점에 할 Detect와 적용할 Buff
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillTimeBuffWrapper> Buffs;
};

/**
 * Skill을 위한 Animation의 정보.
 * Animation과 Animation 작동 시 필요한 Detect, ApplyBuff 정보를 제공.
 * Skill Anim key를 서버에서 받기 위해 Replicate로 설정, Replicate가 닿을 수 있도록 설정
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

	// Anim의 각 Section Index와, Section Index에 맞는 무언가 탐지하고 줄 영향 지정
	// {Section Name, (Section Play 로부터의 시간, 탐지, 효과 정보) } 
	UPROPERTY( NotReplicated, EditAnywhere, meta = ( EditFixedOrder ) )
	TMap<FName, FR4SkillTimeDetectArray> DetectExecutes;

	// Anim의 각 Section Index와, Section Index에 맞는 버프 지정
	// {Section Name, (Section Play 로부터의 시간, 버프 관련 정보) } 
	UPROPERTY( NotReplicated, EditAnywhere, meta = ( EditFixedOrder ) )
	TMap<FName, FR4SkillTimeBuffArray> BuffExecutes;
	
	// Server와 Client 사이 Skill Anim을 구분하기 위한 Key값.
	UPROPERTY( Transient, VisibleInstanceOnly )
	int32 SkillAnimServerKey;
};