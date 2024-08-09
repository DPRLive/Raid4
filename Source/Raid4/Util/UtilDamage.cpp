#include "UtilDamage.h"

#include "../Damage/Calculator/R4DamageCalculatorInterface.h"
#include "../Stat/R4StatBaseComponent.h"

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
		const UObject* cdo = InDamageDesc.CalculatorClass->GetDefaultObject(true);

		if(const IR4DamageCalculatorInterface* damageCalculator = Cast<IR4DamageCalculatorInterface>(cdo))
			return damageCalculator->CalculateDamage(InInstigator, InVictim, InDamageDesc);
	}
	
	FR4DamageReceiveInfo damageInfo;
	damageInfo.bFixedDamage = InDamageDesc.bFixedDamage;
	damageInfo.IncomingDamage = InDamageDesc.Value;

	return damageInfo;
}

/**
*  방어력을 기준으로 방어 감소율을 계산.
*/
float UtilDamage::CalculateReductionByArmor(float InArmor)
{
	float damageReduction = InArmor / (InArmor + Damage::G_DefenseConstant);
	return (1.0f - damageReduction);
}

/**
*  랜덤 데미지 변동율 리턴.
*/
float UtilDamage::GetRandomFactor()
{
	return FMath::RandRange(Damage::G_RandomFactorLower, Damage::G_RandomFactorUpper);
}

/**
*  Critical 판정
*  @param InCriticalChance : 0 ~ 100. %
*  @param OutDamage : 크리티컬 여부에 따라 변동 시킬 Damage
*  @return : 크리티컬 여부
*/
bool UtilDamage::DetermineCritical(float InCriticalChance, float& OutDamage)
{
	if(bool bCritical = FMath::RandRange(0.0f, 1.0f) <= (InCriticalChance / 100.f))
	{
		OutDamage *= Damage::G_CriticalFactor;
		return true;
	}
	
	return false;
}
