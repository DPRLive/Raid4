// Fill out your copyright notice in the Description page of Project Settings.


#include "R4NonPlayerSkillComponent.h"
#include "R4NonPlayerSkillInterface.h"
#include "../R4SkillBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4NonPlayerSkillComponent)

/**
 *  스킬 사용 요청 처리
 *  @return : Skill의 종료를 알리는 Delegate
 */
FSimpleMulticastDelegate* UR4NonPlayerSkillComponent::ActivateAISkill( uint8 InSkillIndex )
{
	if ( SkillInstances.IsValidIndex( InSkillIndex ) )
	{
		if ( IR4NonPlayerSkillInterface* aiSkill = Cast<IR4NonPlayerSkillInterface>( SkillInstances[InSkillIndex] ) )
		{
			aiSkill->ActivateAISkill();
			return &SkillInstances[InSkillIndex]->OnEndSkill;
		}
	}

	return nullptr;
}

/**
 *  사용 가능한 스킬 중, ActivateSkillMinDist가 큰 Skill Index를 반환
 *  @param OutDist : 사용 가능한 스킬을 찾았다면, ActivateSkillMinDist.
 *  @return : 사용 가능한 Skill이 없는 경우, INDEX_NONE 반환.
 */
int32 UR4NonPlayerSkillComponent::GetAvailableMaxDistSkillIndex( float& OutDist ) const
{
	int32 maxIdx = INDEX_NONE; float maxDist = 0.f;
	
	for ( int32 idx = 0; idx < SkillInstances.Num(); idx++ )
	{
		IR4NonPlayerSkillInterface* aiSkill = Cast<IR4NonPlayerSkillInterface>( SkillInstances[idx] );
		if ( aiSkill == nullptr || !SkillInstances[idx]->CanActivateSkill() )
			continue;

		float nowDist = aiSkill->GetActivateSkillMinDist();
		if ( maxIdx == INDEX_NONE || nowDist > maxDist )
		{
			maxIdx = idx;
			maxDist = nowDist;
		}
	}

	OutDist = maxDist;
	return maxIdx;
}
