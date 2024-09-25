// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Buff/R4BuffBase.h"
#include "R4Buff_ForceMove.generated.h"

class UCurveVector;
class UR4CharacterMovementComponent;

/**
 * ForceMove를 적용하는 버프.
 * R4CharacterMovementComp에 의존.
 * Deactivate 시 해당 버프가 이동 시킨 만큼 다시 원래대로 이동
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Buff) )
class RAID4_API UR4Buff_ForceMove : public UR4BuffBase
{
	GENERATED_BODY()

public:
	UR4Buff_ForceMove();
	
public:
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의. 세팅( 버프 효과 적용이 가능한 상태인가 ) 실패 시 false를 꼭 리턴,
	// 클래스 상속 시 추가 정보가 필요하다면 오버라이드 하여 세팅 작업을 추가적으로 진행
	virtual bool SetupBuff(AActor* InInstigator, AActor* InVictim) override;
	
	// 버프를 적용, 실제로 해당 Buff가 적용할 효과 로직을 정의. SetupBuff 후 사용.
	virtual bool ApplyBuff() override;

protected:
	// 버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
	virtual void Deactivate() override;

	// 해당 버프 클래스를 초기 상태로 Reset
	virtual void Reset() override;

private:
	// Force Move시 사용할 타입
	UPROPERTY( EditDefaultsOnly )
	ER4ForceMoveType ForceMoveType;
	
	// 이동시킬 Relative Location
	UPROPERTY( EditDefaultsOnly )
	FVector TargetRelativeLocation;

	// 이동에 걸리는 시간
	UPROPERTY( EditDefaultsOnly )
	float Duration;
	
	// Curve Move시 사용할 Curve Vector Asset
	UPROPERTY( EditDefaultsOnly, meta = (editcondition = "ForceMoveType == ER4ForceMoveType::CurveVector", EditConditionHides) )
	TObjectPtr<UCurveVector> CurveVector;

	// 시작 지점 캐싱
	FVector CachedStartLoc;
	
	// Shield Comp 캐싱
	TWeakObjectPtr<UR4CharacterMovementComponent> CachedMoveComp;
};