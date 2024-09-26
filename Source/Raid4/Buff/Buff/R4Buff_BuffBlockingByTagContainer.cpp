// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_BuffBlockingByTagContainer.h"
#include "../R4BuffManageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_BuffBlockingByTagContainer)

UR4Buff_BuffBlockingByTagContainer::UR4Buff_BuffBlockingByTagContainer()
{
	bDeactivate = true;
	QueryType = EGameplayTagQueryType::Match;
}

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_BuffBlockingByTagContainer::SetupBuff(AActor* InInstigator, AActor* InVictim)
{
	bool bReady = Super::SetupBuff(InInstigator, InVictim);

	// 버프 받을 객체의 BuffManageComp를 캐싱
	if (CachedVictim.IsValid())
		CachedBuffManageComp = CachedVictim->FindComponentByClass<UR4BuffManageComponent>();

	return bReady && CachedBuffManageComp.IsValid();
}

/**
 *  버프를 적용 ( Buff Manage Comp의 Blocking Tag에 Tag를 추가 )
 */
bool UR4Buff_BuffBlockingByTagContainer::ApplyBuff()
{
	if (!Super::ApplyBuff())
		return false;

	if(CachedBuffManageComp.IsValid())
	{
		CachedBuffManageComp->AddBlockingBuffTags(BlockingTagContainer, QueryType);
		return true;
	}
		
	return false;
}

/**
 *  버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
 *  Deactivate 시 적용했던 Blocking Tag를 삭제.
 */
void UR4Buff_BuffBlockingByTagContainer::Deactivate()
{
	Super::Deactivate();

	if(CachedBuffManageComp.IsValid())
		CachedBuffManageComp->RemoveBlockingBuffTags(BlockingTagContainer, QueryType);
}

/**
 *  해당 버프 클래스를 초기 상태로 Reset
 */
void UR4Buff_BuffBlockingByTagContainer::Reset()
{
	Super::Reset();

	CachedBuffManageComp.Reset();
}
