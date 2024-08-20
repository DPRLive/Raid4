// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"
#include "R4Buff_ShieldApplier.generated.h"

class UR4ShieldComponent;

/**
 * Shield를 적용하는 버프.
 * Shield Comp에 의존.
 * Deactivate 시 해당 버프가 적용한 Shield를 해제.
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Buff) )
class RAID4_API UR4Buff_ShieldApplier : public UR4BuffBase
{
	GENERATED_BODY()
	
protected:
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의. 세팅 실패 시 false를 꼭 리턴
	virtual bool PreActivate(AActor* InInstigator, AActor* InVictim, const FR4BuffDesc* InBuffDesc) override;
	
	// 버프가 실제로 할 로직을 정의
	virtual void Activate() override;

	// 버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
	virtual void Deactivate() override;

	// 버프를 Clear하는 로직을 정의
	virtual void Reset() override;

private:
	// Shield Comp 캐싱
	TWeakObjectPtr<UR4ShieldComponent> CachedShieldComp;
};
