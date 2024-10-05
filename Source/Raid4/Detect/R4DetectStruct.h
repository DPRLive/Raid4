#pragma once

#include <Components/PrimitiveComponent.h>
#include "R4DetectStruct.generated.h"

/**
 *  Trace Detect의 실행 타입 
 */
UENUM()
enum class ER4TraceDetectExecutionType : uint8
{
	OneShot			UMETA( DisplayName = "1회 실행" ),
	Interval		UMETA( DisplayName = "간격마다 실행" ),
};

/**
 *  Trace Detect 체크시 사용할 모양
 */
UENUM()
enum class ER4TraceDetectShapeType : uint8
{
	Box			UMETA( DisplayName = "박스" ),
	Sphere		UMETA( DisplayName = "구" ),
	Capsule		UMETA( DisplayName = "캡슐" ),
	Sector		UMETA( DisplayName = "부채꼴" )
};

/**
 *  Detect할 Trace Detect 시 관련 모양 정보 설정
 */
USTRUCT()
struct FR4TraceDetectShapeInfo
{
	GENERATED_BODY()

	FR4TraceDetectShapeInfo()
	: Shape( ER4TraceDetectShapeType::Box )
	, BoxHalfExtent( FVector::ZeroVector )
	, Radius( 0.f )
	, HalfHeight( 0.f )
	, Angle( 0.f )
	{}
	
	// Overlap할 형태 결정
	UPROPERTY( EditAnywhere, Category="Collision", meta =(AllowPrivateAccess = true) )
	ER4TraceDetectShapeType Shape;
	
	// 반지름
	UPROPERTY( EditAnywhere, Category="Collision",
		meta =(  EditCondition = "Shape == ER4TraceDetectShapeType::Box", EditConditionHides, AllowPrivateAccess = true) )
	FVector BoxHalfExtent;
	
	// 반지름
	UPROPERTY( EditAnywhere, Category="Collision",
		meta =( EditCondition = "Shape != ER4TraceDetectShapeType::Box", EditConditionHides, AllowPrivateAccess = true) )
	float Radius;

	// HalfHeight
	UPROPERTY( EditAnywhere, Category="Collision",
		meta =( EditCondition = "Shape == ER4TraceDetectShapeType::Capsule || Shape == ER4TraceDetectShapeType::Sector", EditConditionHides, AllowPrivateAccess = true) )
	float HalfHeight;

	// angle
	UPROPERTY( EditAnywhere, Category="Collision",
		meta =( EditCondition = "Shape == ER4TraceDetectShapeType::Sector", EditConditionHides, AllowPrivateAccess = true) )
	float Angle;
};

/**
 * Detect 요청 시 사용할 정보
 */
USTRUCT( BlueprintType )
struct FR4DetectDesc
{
	GENERATED_BODY()

	FR4DetectDesc()
	: RelativeLoc( FVector::ZeroVector )
	, RelativeRot( FRotator::ZeroRotator )
	, LifeTime( 0.f )
	{}

	// overlap을 체크할 기준점으로부터의 상대 위치.
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	FVector RelativeLoc;
	
	// Overlap시 기준으로부터 상대적인 Rotation. X : Roll, Y : Pitch, Z : Yaw
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	FRotator RelativeRot;

	// Detector의 지속시간, <= 0.f 일 시 1 Tick만 작동.
	UPROPERTY( EditAnywhere, BlueprintReadOnly, meta = ( UIMin = 0.f, ClampMin = 0.f ) )
	float LifeTime;
};

/**
 * Detect한 결과 정보.
 */
USTRUCT( BlueprintType )
struct FR4DetectResult
{
	GENERATED_BODY()

	FR4DetectResult()
	: Location(FVector_NetQuantize::ZeroVector)
	, DetectedComponent(nullptr)
	, DetectedActor(nullptr)
	{}

	FR4DetectResult(const FVector& InLoc, UPrimitiveComponent* InDetectedComp, AActor* InDetectedActor)
	: Location(InLoc)
	, DetectedComponent(InDetectedComp)
	, DetectedActor(InDetectedActor)
	{}
	
	// 발견한 위치
	UPROPERTY( Transient, BlueprintReadOnly )
	FVector_NetQuantize Location;

	// 발견된 Component
	UPROPERTY( Transient, BlueprintReadOnly )
	TWeakObjectPtr<UPrimitiveComponent> DetectedComponent;

	// 발견된 Actor
	UPROPERTY( Transient, BlueprintReadOnly )
	TWeakObjectPtr<AActor> DetectedActor;
};

/**
 * No Authority Detect시 Replicate할 정보.
 */
USTRUCT( Atomic )
struct FR4NoAuthDetectEnableInfo
{
	GENERATED_BODY()

	FR4NoAuthDetectEnableInfo()
	: bEnable( false )
	, EnableServerTime( -1.f )
	, LifeTime( 0.f )
	{}
	
	// 활성화 상태인지
	UPROPERTY( Transient, VisibleInstanceOnly )
	uint8 bEnable:1;

	// Enable 된 시간.
	UPROPERTY( Transient, VisibleInstanceOnly )
	float EnableServerTime;

	// Life Time
	UPROPERTY( Transient, VisibleInstanceOnly )
	float LifeTime;
};