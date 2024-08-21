// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"
#include "../../Value/R4ValueSelector.h"
#include "../../Damage/R4DamageStruct.h"
#include "R4Buff_DamageApplier.generated.h"

/**
 * Damage를 입히는 Buff.
 * Value를 DamageDesc의 Value로 사용.
 * Damage Interface를 거쳐서 데미지를 전달.
 * Deactivate 불가.
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Buff), HideCategories=(Deactivate) )
class RAID4_API UR4Buff_DamageApplier : public UR4BuffBase
{
	GENERATED_BODY()

public:
	UR4Buff_DamageApplier();

public:
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의. 세팅( 버프 효과 적용이 가능한 상태인가 ) 실패 시 false를 꼭 리턴,
	// 클래스 상속 시 추가 정보가 필요하다면 오버라이드 하여 세팅 작업을 추가적으로 진행
	virtual bool SetupBuff(AActor* InInstigator, AActor* InVictim) override;
	
	// 버프를 적용, 실제로 해당 Buff가 적용할 효과 로직을 정의. SetupBuff 후 사용.
	virtual bool ApplyBuff() override;

private:
	// DamageDesc의 Value로 사용.
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FR4ValueSelector Value;
	
	// 가할 데미지 관련 정보를 설정. Value를 DamageDesc의 Value로 사용.
	// 기존 DamageApplyDesc의 Value값은 사용되지 않음.
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FR4DamageApplyDesc DamageApplyDesc;
};
