// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_ShieldApplier.h"
#include "../../Shield/R4ShieldComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_ShieldApplier)

UR4Buff_ShieldApplier::UR4Buff_ShieldApplier()
{
	Value = FR4ValueSelector();
	bDeactivate = true;
}

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_ShieldApplier::SetupBuff(AActor* InInstigator, AActor* InVictim)
{
	bool bReady = Super::SetupBuff(InInstigator, InVictim);

	// 버프 받을 객체의 Shield Comp를 캐싱
	if(CachedVictim.IsValid())
		CachedShieldComp = CachedVictim->FindComponentByClass<UR4ShieldComponent>();

	return bReady && CachedShieldComp.IsValid();
}

/**
 *  버프를 적용 ( 방어막을 적용 )
 */
bool UR4Buff_ShieldApplier::ApplyBuff()
{
	if(!Super::ApplyBuff())
		return false;
	
	if(CachedShieldComp.IsValid())
	{
		// 피연산 값 계산
		float value = Value.GetValue(CachedInstigator.Get(), CachedVictim.Get());

		// 쉴드 추가.
		CachedShieldComp->AddShield(this, value);

		return true;
	}

	return false;
}

/**
 *  버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
 *  Deactivate 시 해당 버프가 적용한 Shield를 해제.
 */
void UR4Buff_ShieldApplier::Deactivate()
{
	Super::Deactivate();

	// 쉴드 제거.
	if(CachedShieldComp.IsValid())
		CachedShieldComp->RemoveShieldAll(this);
}

/**
 *  버프 종료 시 Clear하는 로직을 정의
 */
void UR4Buff_ShieldApplier::Reset()
{
	Super::Reset();

	CachedShieldComp.Reset();
}
