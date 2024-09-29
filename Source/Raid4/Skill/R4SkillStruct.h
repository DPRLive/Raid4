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
	
	// Detector를 Attach할 것인지?
	// Owner가 Character이고 SkeletalMesh를 가지고 있어야 함.
	UPROPERTY( EditAnywhere )
	uint8 bAttachToMesh:1;

	// Attach시 Attack할 Owner의 Skeletal Mesh Socket Name
	UPROPERTY( EditAnywhere, meta = ( EditCondition = "bAttachToMesh", EditConditionHides ) )
	FName MeshSocketName;
	
	// 탐지에 관한 Parameter.
	UPROPERTY( EditAnywhere )
	FR4DetectDesc DetectDesc;
};

/**
 * Skill에서 주는 Buff에 대한 정보
 * 데미지도 버프로 적용 시키면 됨
 */
// USTRUCT()
// struct FR4SkillBuffInfo
// {
// 	GENERATED_BODY()
//
// 	FR4SkillBuffInfo()
// 	: BuffNetFlag( 0 )
// 	, BuffClass( nullptr )
// 	, BuffSetting( FR4BuffSettingDesc() )
// 	{}
//
// 	// Buff를 적용할 Network 정책을 설정
// 	UPROPERTY( EditAnywhere, meta = ( Bitmask, BitmaskEnum = "/Script/Raid4.ER4NetworkFlag_NoSimulated" ) )
// 	uint8 BuffNetFlag;
// 	
//
// };

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
	TArray<FR4SkillDetectBuffInfo> OnBeginDetectBuffs;
	
	// OnEndDetect 시 적용할 버프
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillDetectBuffInfo> OnEndDetectBuffs;
};

/**
 * Notify Number와, Detect, Buff Wrapper
 */
USTRUCT()
struct FR4NotifyDetectWrapper
{
	GENERATED_BODY()

	FR4NotifyDetectWrapper()
	: NotifyNumber( INDEX_NONE )
	, DetectEffect( FR4SkillDetectBuffWrapper() )
	{}

	FR4NotifyDetectWrapper( int32 InNotifyNumber )
	: NotifyNumber( InNotifyNumber )
	, DetectEffect( FR4SkillDetectBuffWrapper() )
	{}
	
	// Notify Number
	UPROPERTY( VisibleAnywhere )
	int32 NotifyNumber;

	// 해당 Notify가 할 Detect와 Buff
	UPROPERTY( EditAnywhere )
	FR4SkillDetectBuffWrapper DetectEffect;
};

/**
 * Notify Number와 Buff Wrapper
 */
USTRUCT()
struct FR4NotifyBuffWrapper
{
	GENERATED_BODY()

	FR4NotifyBuffWrapper()
	: NotifyNumber( INDEX_NONE )
	, BuffSetting( FR4BuffSettingDesc() )
	, bApplyOwner( false )
	{}

	FR4NotifyBuffWrapper( int32 InNotifyNumber )
	: NotifyNumber( InNotifyNumber )
	, BuffSetting( FR4BuffSettingDesc() )
	, bApplyOwner( false )
	{}
	
	// Notify Number
	UPROPERTY( VisibleAnywhere )
	int32 NotifyNumber;

	// 해당 Notify가 적용할 버프 클래스
	UPROPERTY( EditAnywhere )
	TSubclassOf<UR4BuffBase> BuffClass;

	// 해당 Notify가 버프의 세팅
	UPROPERTY( EditAnywhere )
	FR4BuffSettingDesc BuffSetting;

	// Server 뿐만 아니라 Owner Client에서도 같이 적용이 필요한지?
	UPROPERTY( EditAnywhere )
	uint8 bApplyOwner:1;
};

/**
 * Skill을 위한 Animation의 정보.
 * Animation과 Animation 작동 시 필요한 히트 체크 정보를 제공.
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

	// Anim의 각 Notify와, Notify 번호에 맞는 무언가 탐지하고 줄 영향 지정
	// {Notify index ( AnimMontage에서 몇번째 Notify인지 ), 탐지 및 효과 정보} 
	UPROPERTY( NotReplicated, EditAnywhere, meta = ( EditFixedOrder ) )
	TArray<FR4NotifyDetectWrapper> DetectNotifies;

	// Anim의 각 Notify와, Notify 번호에 맞는 버프 지정
	// {Notify index ( AnimMontage에서 몇번째 Notify인지 ), 버프 관련 정보} 
	UPROPERTY( NotReplicated, EditAnywhere, meta = ( EditFixedOrder ) )
	TArray<FR4NotifyBuffWrapper> BuffNotifies;
	
	// Server와 Client 사이 Skill Anim을 구분하기 위한 Key값.
	UPROPERTY( Transient, VisibleInstanceOnly )
	uint32 SkillAnimServerKey;
};