#pragma once

#include "../Detect/R4DetectStruct.h"
#include "../Buff/R4BuffStruct.h"

#include "R4SkillStruct.generated.h"

class UR4BuffBase;
class UAnimMontage;

/**
 * Detector의 Spawn 정책을 설정
 * Server Only : 서버에서만 생성
 * DummyAndAuthority : Owner 클라이언트에서 Dummy, Server에서 Authority 생성
 */
UENUM( BlueprintType )
enum class ER4SkillDetectorSpawnType : uint8
{
	ServerOnly			UMETA( DisplayName = "서버에서만 생성" ),
	DummyAndAuthority	UMETA( DisplayName = "Owner Dummy, Server Authority" )
};

/**
 * Skill에서 진행할 탐지에 관한 정보.
 * 사용 시 Replicate가 닿을 수 있도록 설정
 */
USTRUCT()
struct FR4SkillDetectInfo
{
	GENERATED_BODY()

	FR4SkillDetectInfo()
	: DetectClass( nullptr )
	, DetectorSpawnType( ER4SkillDetectorSpawnType::ServerOnly )
	, bAttachToMesh( false )
	, MeshSocketName( NAME_None )
	, DetectDesc( FR4DetectDesc() )
	, DetectorServerKey( Skill::G_InvalidDetectorKey )
	{}
	
	// 어떤 클래스로 Detect를 진행할 것인지?
	// Dummy가 필요한 경우 (bHasVisual) Detect Class를 Replicate 설정해야함! 
	UPROPERTY( NotReplicated, EditAnywhere, meta = ( MustImplement = "R4DetectorInterface" ) )
	TSubclassOf<AActor> DetectClass;
	
	// Detector의 Spawn 정책을 설정
	UPROPERTY( NotReplicated, EditAnywhere )
	ER4SkillDetectorSpawnType DetectorSpawnType;
	
	// Detector를 Attach할 것인지?
	// Owner가 Character이고 SkeletalMesh를 가지고 있어야 함.
	UPROPERTY( NotReplicated, EditAnywhere )
	uint8 bAttachToMesh:1;

	// Attach시 Attack할 Owner의 Skeletal Mesh Socket Name
	UPROPERTY( NotReplicated, EditAnywhere, meta = ( EditCondition = "bAttachToMesh", EditConditionHides ) )
	FName MeshSocketName;
	
	// 탐지에 관한 Parameter.
	UPROPERTY( NotReplicated, EditAnywhere )
	FR4DetectDesc DetectDesc;

	// Server와 Client 사이 Detector를 구분하기 위한 Key값.
	UPROPERTY( Transient, VisibleInstanceOnly )
	uint32 DetectorServerKey;
};

/**
 * Skill이 Detect시 줄 버프 대한 정보.
 * 데미지도 버프로 적용 시키면 됨
 */
USTRUCT()
struct FR4SkillDetectBuffInfo
{
	GENERATED_BODY()

	FR4SkillDetectBuffInfo()
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
 * Skill에서 Detect시 줄 영향에 대한 정보
 */
USTRUCT()
struct FR4SkillDetectEffectInfo
{
	GENERATED_BODY()
	
	// OnDetect 시 적용할 버프 (서버에서만 적용)
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillDetectBuffInfo> Server_OnBeginDetectBuffs;
	
	// OnEndDetect 시 적용할 버프 (서버에서만 적용)
	UPROPERTY( EditAnywhere )
	TArray<FR4SkillDetectBuffInfo> Server_OnEndDetectBuffs;
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
	, EffectInfo( FR4SkillDetectEffectInfo() )
	{}
	
	// 탐지할 방법
	UPROPERTY( EditAnywhere )
	FR4SkillDetectInfo DetectInfo;

	// 탐지 시 줄 영향
	UPROPERTY( NotReplicated, EditAnywhere )
	FR4SkillDetectEffectInfo EffectInfo;
};

/**
 * Notify Number와, Detect, Effect Wrapper
 */
USTRUCT()
struct FR4NotifyDetectWrapper
{
	GENERATED_BODY()

	FR4NotifyDetectWrapper()
	: NotifyNumber( INDEX_NONE )
	, DetectEffect( FR4DetectEffectWrapper() )
	{}

	FR4NotifyDetectWrapper(int32 InNotifyNumber)
	: NotifyNumber( InNotifyNumber )
	, DetectEffect( FR4DetectEffectWrapper() )
	{}
	
	// Notify Number
	UPROPERTY( NotReplicated, VisibleAnywhere )
	int32 NotifyNumber;

	// 해당 Notify가 할 Detect와 Effect
	UPROPERTY( EditAnywhere )
	FR4DetectEffectWrapper DetectEffect;
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
	UPROPERTY( EditAnywhere, meta = ( EditFixedOrder ) )
	TArray<FR4NotifyDetectWrapper> DetectNotifies;

	// Anim의 각 Notify와, Notify 번호에 맞는 버프 지정
	// {Notify index ( AnimMontage에서 몇번째 Notify인지 ), 버프 관련 정보} 
	UPROPERTY( NotReplicated, EditAnywhere, meta = ( EditFixedOrder ) )
	TArray<FR4NotifyBuffWrapper> BuffNotifies;
	
	// Server와 Client 사이 Skill Anim을 구분하기 위한 Key값.
	UPROPERTY( Transient, VisibleInstanceOnly )
	uint32 SkillAnimServerKey;
};