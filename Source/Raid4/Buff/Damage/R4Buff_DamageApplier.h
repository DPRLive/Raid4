// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"
#include "../../Damage/R4DamageStruct.h"
#include "R4Buff_DamageApplier.generated.h"

/**
 * Damage를 입히는 Buff.
 * Damage Interface를 거쳐서 데미지를 전달.
 * BuffDesc의 Value를 DamageDesc의 Value로 사용.
 * Deactivate 불가.
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Buff), HideCategories=(Deactivate) )
class RAID4_API UR4Buff_DamageApplier : public UR4BuffBase
{
	GENERATED_BODY()

public:
	UR4Buff_DamageApplier();

protected:
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의.
	virtual bool PreActivate(AActor* InInstigator, AActor* InVictim, const FR4BuffDesc* InBuffDesc = nullptr) override;
	
	// 버프가 실제로 할 로직을 정의
	virtual void Activate() override;

private:
	// 가할 데미지 관련 정보를 설정. BuffDesc의 Value를 DamageDesc의 Value로 사용.
	// 기존 DamageApplyDesc의 Value값은 사용되지 않음.
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FR4DamageApplyDesc DamageApplyDesc;
};
