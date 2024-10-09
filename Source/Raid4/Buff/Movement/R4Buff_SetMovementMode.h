// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"
#include "R4Buff_SetMovementMode.generated.h"

class UCharacterMovementComponent;

/**
 * Movement Mode를 변경하는 버프.
 * Movement Mode 변경 버프의 경우, 중첩 적용에 주의할 것.
 * CharacterMovementComp에 의존.
 * Deactivate 시 default movement mode로 변경
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Buff) )
class RAID4_API UR4Buff_SetMovementMode : public UR4BuffBase
{
	GENERATED_BODY()

public:
	UR4Buff_SetMovementMode();
	
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
	// 변경할 Force Movement Mode
	UPROPERTY( EditDefaultsOnly )
	TEnumAsByte<EMovementMode> MovementMode;
	
	// Move Comp 캐싱
	TWeakObjectPtr<UCharacterMovementComponent> CachedMoveComp;
};
