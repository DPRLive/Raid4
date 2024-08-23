// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_BuffRemoverByTagContainer.h"
#include "../R4BuffManageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_BuffRemoverByTagContainer)

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_BuffRemoverByTagContainer::SetupBuff(AActor* InInstigator, AActor* InVictim)
{
	bool bReady = Super::SetupBuff(InInstigator, InVictim);

	// 버프 받을 객체의 BuffManageComp를 캐싱
	if (CachedVictim.IsValid())
		CachedBuffManageComp = CachedVictim->FindComponentByClass<UR4BuffManageComponent>();

	return bReady && CachedBuffManageComp.IsValid();
}

/**
 *  버프를 적용 ( Tag로 쿼리하여 버프를 강제로 해제 )
 */
bool UR4Buff_BuffRemoverByTagContainer::ApplyBuff()
{
	if (!Super::ApplyBuff())
		return false;

	if(CachedBuffManageComp.IsValid())
	{
		CachedBuffManageComp->Server_RemoveBuffAllByTagContainer(QueryTagContainer, QueryType);
		return true;
	}
		
	return false;
}

/**
 *  해당 버프 클래스를 초기 상태로 Reset
 */
void UR4Buff_BuffRemoverByTagContainer::Reset()
{
	Super::Reset();

	CachedBuffManageComp.Reset();
}
