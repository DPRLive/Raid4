// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_SetRotateSetting.h"

#include <GameFramework/CharacterMovementComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_SetRotateSetting)

UR4Buff_SetRotateSetting::UR4Buff_SetRotateSetting()
{
	bOrientRotationToMovement = false;
	bUseControllerDesiredRotation = false;
	CachedIsOrientRotationToMovement = false;
	CachedIsUseControllerDesiredRotation = false;
}

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_SetRotateSetting::SetupBuff( AActor* InInstigator, AActor* InVictim )
{
	bool bReady = Super::SetupBuff(InInstigator, InVictim);

	// 버프 받을 객체의 Movement Comp를 캐싱
	if ( CachedVictim.IsValid() )
		CachedMoveComp = CachedVictim->FindComponentByClass<UCharacterMovementComponent>();

	return bReady && CachedMoveComp.IsValid();
}

/**
 *  버프를 적용 ( Rotate Setting을 적용 )
 */
bool UR4Buff_SetRotateSetting::ApplyBuff()
{
	if ( !Super::ApplyBuff() )
		return false;

	if ( CachedMoveComp.IsValid() )
	{
		// 기존 값 캐싱
		CachedIsOrientRotationToMovement = CachedMoveComp->bOrientRotationToMovement;
		CachedIsUseControllerDesiredRotation = CachedMoveComp->bUseControllerDesiredRotation;

		// 새로 값 설정
		CachedMoveComp->bOrientRotationToMovement = bOrientRotationToMovement;
		CachedMoveComp->bUseControllerDesiredRotation = bUseControllerDesiredRotation;
		
		return true;
	}

	return false;
}

/**
 *  버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
 *  버프 적용 직전 값으로 다시 적용
 */
void UR4Buff_SetRotateSetting::Deactivate()
{
	Super::Deactivate();

	if ( CachedMoveComp.IsValid() )
	{
		// 캐싱 해두었던 값으로 다시 설정
		CachedMoveComp->bOrientRotationToMovement = CachedIsOrientRotationToMovement;
		CachedMoveComp->bUseControllerDesiredRotation = CachedIsUseControllerDesiredRotation;
	}
}

void UR4Buff_SetRotateSetting::Reset()
{
	Super::Reset();

	CachedIsOrientRotationToMovement = false;
	CachedIsUseControllerDesiredRotation = false;
	CachedMoveComp.Reset();
}
