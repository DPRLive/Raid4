// Fill out your copyright notice in the Description page of Project Settings.


#include "R4BuffBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4BuffBase)

UR4BuffBase::UR4BuffBase()
{
	bDeactivate = false;
	BuffTag = FGameplayTag::EmptyTag;
}

/**
 *  Pool에 반납 전 로직 처리
 */
void UR4BuffBase::PreReturnPoolObject()
{
	Reset();
}

/**
 *  버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의.
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @return : 세팅 성공 실패 여부 ( 버프 효과 적용이 가능한 상태인가 )
 */
bool UR4BuffBase::SetupBuff(AActor* InInstigator, AActor* InVictim)
{
	CachedInstigator = InInstigator;
	CachedVictim = InVictim;
	
	return true;
}

/**
 *  버프를 제거, 필요 시 제거 로직 실행
 */
void UR4BuffBase::RemoveBuff()
{
	// 복구 로직이 필요한 경우
	if(bDeactivate)
		Deactivate();
}

/**
 *  해당 버프 클래스를 초기 상태로 Reset
 */
void UR4BuffBase::Reset()
{
	CachedInstigator.Reset();
	CachedVictim.Reset();
}