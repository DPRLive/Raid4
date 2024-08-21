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
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_DamageApplier::SetupBuff(AActor* InInstigator, AActor* InVictim)
{
	bool bReady = Super::SetupBuff(InInstigator, InVictim);
	
	if(!InVictim->GetClass())
		return false;
	
	// 상대가 데미지를 받을 수 있는 객체인지 확인
	return bReady && InVictim->GetClass()->ImplementsInterface(UR4DamageReceiveInterface::StaticClass());
}

/**
 *  버프를 적용 ( 데미지를 적용 )
 */
bool UR4Buff_DamageApplier::ApplyBuff()
{
	if(!Super::ApplyBuff())
		return false;

	// 데미지 인터페이스를 통해 데미지 전달
	if(IR4DamageReceiveInterface* victim = Cast<IR4DamageReceiveInterface>(CachedVictim))
	{
		// 데미지 계산
		FR4DamageReceiveInfo damageInfo = UtilDamage::CalculateDamageReceiveInfo(CachedInstigator.Get(), CachedVictim.Get(), DamageApplyDesc);

		// 음수의 데미지를 가하려는 경우 경고
		if(damageInfo.IncomingDamage < 0.f)
			LOG_WARN(R4Log, TEXT("Warning! [%s] : Try to apply negative damage[%f]."), *GetName(), damageInfo.IncomingDamage);
		
		victim->ReceiveDamage(CachedInstigator.Get(), damageInfo);

		return true;
	}

	return false;
}