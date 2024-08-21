// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"
#include "../../Value/R4ValueSelector.h"

#include "R4Buff_ShieldApplier.generated.h"

class UR4ShieldComponent;

/**
 * Shield를 적용하는 버프.
 * Value를 Shield의 양으로 사용
 * Shield Comp에 의존.
 * Deactivate 시 해당 버프가 적용한 Shield를 해제.
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Buff) )
class RAID4_API UR4Buff_ShieldApplier : public UR4BuffBase
{
	GENERATED_BODY()

public:
	UR4Buff_ShieldApplier();
	
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
	// Shield의 양으로 사용
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FR4ValueSelector Value;
	
	// Shield Comp 캐싱
	TWeakObjectPtr<UR4ShieldComponent> CachedShieldComp;
};
