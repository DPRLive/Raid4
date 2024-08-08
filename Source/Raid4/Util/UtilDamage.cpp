#include "UtilDamage.h"

#include "../Damage/Calculator/R4DamageCalculatorInterface.h"

/**
*  R4DamageApplyDesc를 기반으로 데미지를 계산하여 R4DamageReceiveInfo를 산출
*  @param InInstigator : 데미지를 가하는(Apply) 객체
*  @param InVictim : 데미지를 입는(Receive) 객체
*  @param InDamageDesc : 데미지에 관한 정보.
*/
FR4DamageReceiveInfo UtilDamage::CalculateDamageReceiveInfo(const AActor* InInstigator, const AActor* InVictim, const FR4DamageApplyDesc& InDamageDesc)
{
	// Calculator class가 있다면 적용.
	if(IsValid(InDamageDesc.CalculatorClass) &&
		ensureMsgf(InDamageDesc.CalculatorClass->ImplementsInterface(UR4DamageCalculatorInterface::StaticClass()),
		TEXT("DamageCalculator class must implement R4DamageCalculatorInterface.")))
	{
		// cdo를 통해 execute
		UObject* cdo = InDamageDesc.CalculatorClass->GetDefaultObject(true);

		if(IR4DamageCalculatorInterface* damageCalculator = Cast<IR4DamageCalculatorInterface>(cdo))
			return damageCalculator->CalculateDamage(InInstigator, InVictim, InDamageDesc);
	}
	
	FR4DamageReceiveInfo damageInfo;
	damageInfo.bFixedDamage = InDamageDesc.bFixedDamage;
	damageInfo.IncomingDamage = InDamageDesc.Value;

	return damageInfo;
}
