#pragma once

#include "R4BuffStruct.generated.h"

class UR4BuffBase;

/**
 *  버프 적용 방식
 */
UENUM( BlueprintType )
enum class EBuffMode : uint8
{
	Instant			UMETA( DisplayName = "즉시 적용" ),
	Interval		UMETA( DisplayName = "일정 시간마다 적용" ),
};

/**
 *  버프 지속 시간 방식
 */
UENUM( BlueprintType )
enum class EBuffDurationType : uint8
{
	OneShot			UMETA( DisplayName = "일회성" ),
	Duration		UMETA( DisplayName = "지속 시간" ),
	Infinite		UMETA( DisplayName = "무한" ),
};

/**
 * Buff 사용에 관한 설정 Desc.
 */
USTRUCT ( BlueprintType )
struct RAID4_API FR4BuffSettingDesc
{
	GENERATED_BODY()

	FR4BuffSettingDesc()
		: BuffMode(EBuffMode::Instant)
		, IntervalTime(0.f)
		, BuffDurationType(EBuffDurationType::OneShot)
		, Duration(0.f)
	{ }

	// 버프 적용 방식
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	EBuffMode BuffMode;
	
	// 버프가 간격을 두고 실행이 될때, 간격의 시간을 설정
	UPROPERTY( EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="BuffMode == EBuffMode::Interval && BuffDurationType != EBuffDurationType::OneShot",
			EditConditionHides, ClampMin = "0.01", UIMin = "0.01"))
	float IntervalTime;
	
	// 버프 지속 시간 방식
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	EBuffDurationType BuffDurationType;
	
	// 버프의 '지속 시간'으로 사용, 버프에 따라 다르게 동작할 수 있으니 확인 후 사용
	UPROPERTY( EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="BuffDurationType == EBuffDurationType::Duration",
			EditConditionHides, ClampMin = "0.0", UIMin = "0.0"))
	float Duration;
};


/**
 * '걸린 버프' 정보를 관리.
 */
USTRUCT()
struct FAppliedBuffInfo
{
	GENERATED_BODY()

	FAppliedBuffInfo ()
		: BuffClass( nullptr )
		, BuffSettingDesc( FR4BuffSettingDesc() )
		, FirstAppliedServerTime( 0.f )
		, LastAppliedServerTime( 0.f )
		, BuffInstance( nullptr )
	{ }
	
	// 걸린 버프 클래스
	UPROPERTY( VisibleInstanceOnly, Transient )
	TSubclassOf<UR4BuffBase> BuffClass;

	// 버프 세팅 정보
	UPROPERTY( VisibleInstanceOnly, Transient )
	FR4BuffSettingDesc BuffSettingDesc;
	
	// 버프가 걸리기 시작한 시간 (서버 타임)
	UPROPERTY( VisibleInstanceOnly, Transient )
	double FirstAppliedServerTime;

	// 버프 효과가 마지막에 발동한 시간 (서버 타임)
	UPROPERTY( VisibleInstanceOnly, Transient )
	double LastAppliedServerTime;
	
	// 버프의 인스턴스.
	UPROPERTY( NotReplicated, VisibleInstanceOnly, Transient )
	TObjectPtr<UR4BuffBase> BuffInstance;
};

/**
 * 서버에서 걸린 버프의 정보.
 */
USTRUCT()
struct FServerAppliedBuffInfo
{
	GENERATED_BODY()

	FServerAppliedBuffInfo ()
		: BuffClass( nullptr )
		, AppliedServerTime( 0.f )
		, Duration( 0.f )
		, Server_BuffInstance( nullptr )
	{ }
	
	// 걸린 버프 클래스
	UPROPERTY( VisibleInstanceOnly, Transient )
	TSubclassOf<UR4BuffBase> BuffClass;
	
	// 버프가 걸리기 시작한 시간 (서버 타임)
	UPROPERTY( VisibleInstanceOnly, Transient )
	float AppliedServerTime;

	// 지속 시간 ( < 0 이면 무한대 )
	UPROPERTY( VisibleInstanceOnly, Transient )
	float Duration;
	
	// 서버의 버프 인스턴스. (서버에서의 버프 식별용, 서버에서만 존재)
	UPROPERTY( NotReplicated, VisibleInstanceOnly, Transient )
	TWeakObjectPtr<UR4BuffBase> Server_BuffInstance;
};