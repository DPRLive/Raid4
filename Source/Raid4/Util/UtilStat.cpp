#include "UtilStat.h"

/**
*  방어력을 기준으로 감소된 피해량 비율을 계산.
*  @param InArmor : 방어력
*  @return : 감소된 피해량의 비율. 그대로 곱해주면 됨
*/
float UtilStat::GetDamageReductionFactor( float InArmor )
{
	float damageReduction = InArmor / (InArmor + Stat::G_ArmorConstant);
	return ( 1.0f - damageReduction );
}

/**
*  쿨타임 감소를 기준으로 감소된 쿨타임 비율을 계산.
*  @param InCoolDownReduction : 쿨타임 감소 ( 백분율, 0 ~ 50 사이로 조절됨 )
*  @return : 감소된 쿨타임의 비율. 그대로 곱해주면 됨.
*/
float UtilStat::GetCoolDownReductionFactor( float InCoolDownReduction )
{
	float coolDownPer = FMath::Clamp( InCoolDownReduction, 0.f, Stat::G_CoolDownReductionUpper );
	return 1 - ( coolDownPer / 100.f );
}