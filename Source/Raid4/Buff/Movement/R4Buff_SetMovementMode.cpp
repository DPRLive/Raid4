// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_SetMovementMode.h"

#include <GameFramework/CharacterMovementComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_SetMovementMode)

UR4Buff_SetMovementMode::UR4Buff_SetMovementMode()
{
	bDeactivate = false;
	MovementMode = MOVE_Walking;
	CachedMoveComp = nullptr;
}

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_SetMovementMode::SetupBuff( AActor* InInstigator, AActor* InVictim )
{
	bool bReady = Super::SetupBuff(InInstigator, InVictim);

	// 버프 받을 객체의 Movement Comp를 캐싱
	if(CachedVictim.IsValid())
		CachedMoveComp = CachedVictim->FindComponentByClass<UCharacterMovementComponent>();

	return bReady && CachedMoveComp.IsValid();
}

/**
 *  버프를 적용 ( Movement mode를 적용 )
 */
bool UR4Buff_SetMovementMode::ApplyBuff()
{
	if ( !Super::ApplyBuff() )
		return false;

	if ( CachedMoveComp.IsValid() )
	{
		CachedMoveComp->SetMovementMode( MovementMode );
		return true;
	}

	return false;
}

/**
 *  버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
 *  Deactivate 시 default movement mode로 변경
 */
void UR4Buff_SetMovementMode::Deactivate()
{
	Super::Deactivate();

	//  default movement mode로 변경
	if ( CachedMoveComp.IsValid() )
		CachedMoveComp->SetDefaultMovementMode( );
}

void UR4Buff_SetMovementMode::Reset()
{
	Super::Reset();

	CachedMoveComp.Reset();
}
