#pragma once

/**
 * Stat 관련 계산을 도와주는 Util Stat.
 */
namespace UtilStat
{
	// 방어력을 기준으로 감소된 피해량 비율을 계산.
	float GetDamageReductionFactor( float InArmor );

	// 쿨타임 감소를 기준으로 감소된 쿨타임 비율을 계산.
	float GetCoolDownReductionFactor( float InCoolDownReduction );
}