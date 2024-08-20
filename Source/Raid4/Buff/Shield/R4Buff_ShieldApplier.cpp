// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_ShieldApplier.h"
#include "../../Shield/R4ShieldComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_ShieldApplier)

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @param InBuffDesc : 버프 적용 시 기본 클래스에서 설정한 값 말고 다른 값이 필요한 경우 적용.
 *  BuffDesc.Value : Shield Comp에 넘겨 줄 방어막 양으로 사용
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_ShieldApplier::PreActivate(AActor* InInstigator, AActor* InVictim, const FR4BuffDesc* InBuffDesc)
{
	bool bReady = Super::PreActivate(InInstigator, InVictim, InBuffDesc);

	// 버프 받을 객체의 Shield Comp를 캐싱
	if(CachedVictim.IsValid())
		CachedShieldComp = CachedVictim->FindComponentByClass<UR4ShieldComponent>();

	return bReady && CachedShieldComp.IsValid();
}

/**
 *  버프를 적용 ( 방어막을 적용 )
 */
void UR4Buff_ShieldApplier::Activate()
{
	Super::Activate();

	// 쉴드 추가.
	if(CachedShieldComp.IsValid())
		CachedShieldComp->AddShield(this, BuffDesc.Value);
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
