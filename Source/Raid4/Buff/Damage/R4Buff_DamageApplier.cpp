// Fill out your copyright notice in the Description page of Project Settings.

#include "R4Buff_DamageApplier.h"
#include "../../Damage/R4DamageReceiveInterface.h"
#include "../../Util/UtilDamage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_DamageApplier)

UR4Buff_DamageApplier::UR4Buff_DamageApplier()
{
	DamageApplyDesc = FR4DamageApplyDesc();
}

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용.
 *  BuffDesc.Value : FR4DamageApplyDesc의 Value 값.
 */
bool UR4Buff_DamageApplier::PreActivate(AActor* InInstigator, AActor* InVictim, const FR4BuffDesc* InBuffDesc)
{
	bool bReady = Super::PreActivate(InInstigator, InVictim, InBuffDesc);
	
	if(!InVictim->GetClass())
		return false;
	
	// 상대가 데미지를 받을 수 있는 객체인지 확인
	return bReady && InVictim->GetClass()->ImplementsInterface(UR4DamageReceiveInterface::StaticClass());
}

void UR4Buff_DamageApplier::Activate()
{
	Super::Activate();

	// 데미지 인터페이스를 통해 데미지 전달
	if(IR4DamageReceiveInterface* victim = Cast<IR4DamageReceiveInterface>(CachedVictim))
	{
		// BuffDesc의 Value로 DamageApplyDesc의 Value를 설정
		DamageApplyDesc.Value = BuffDesc.Value;
		
		// 데미지 계산
		FR4DamageReceiveInfo damageInfo = UtilDamage::CalculateDamageReceiveInfo(CachedInstigator.Get(), CachedVictim.Get(), DamageApplyDesc);
		victim->ReceiveDamage(CachedInstigator.Get(), damageInfo);
	}
}