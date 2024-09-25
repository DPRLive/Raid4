// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_ForceMove.h"

#include "../../Movement/R4CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_ForceMove)

UR4Buff_ForceMove::UR4Buff_ForceMove()
{
	bDeactivate = false;
	ForceMoveType = ER4ForceMoveType::Linear;
	TargetRelativeLocation = FVector::ZeroVector;
	Duration = 0.f;
	CachedStartLoc = FVector::ZeroVector;
}

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_ForceMove::SetupBuff(AActor* InInstigator, AActor* InVictim)
{
	bool bReady = Super::SetupBuff(InInstigator, InVictim);

	// 버프 받을 객체의 Movement Comp를 캐싱
	if(CachedVictim.IsValid())
		CachedMoveComp = CachedVictim->FindComponentByClass<UR4CharacterMovementComponent>();

	return bReady && CachedMoveComp.IsValid();
}

/**
 *  버프를 적용 ( Force Move를 적용 )
 */
bool UR4Buff_ForceMove::ApplyBuff()
{
	if ( !Super::ApplyBuff() )
		return false;

	if ( CachedMoveComp.IsValid() && CachedVictim.IsValid() )
	{
		// Start, Target Location 계산
		CachedStartLoc = CachedVictim->GetActorLocation();
		FVector targetLoc = CachedVictim->GetActorTransform().TransformPosition( TargetRelativeLocation );

		// 타입에 맞춰 Force Move
		if(ForceMoveType == ER4ForceMoveType::Linear)
			CachedMoveComp->SetForceMovementByLinear_Local( targetLoc, Duration );
		else if(ForceMoveType == ER4ForceMoveType::CurveVector)
			CachedMoveComp->SetForceMovementByCurve_Local( targetLoc, Duration, CurveVector, false );
		
		return true;
	}

	return false;
}

/**
 *  버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
 *  Deactivate 시 해당 버프가 이동 시킨 만큼 다시 원래대로 이동
 */
void UR4Buff_ForceMove::Deactivate()
{
	Super::Deactivate();

	// 원래대로 이동
	if ( CachedMoveComp.IsValid() && CachedVictim.IsValid() )
	{
		// 타입에 맞춰 Force Move
		if(ForceMoveType == ER4ForceMoveType::Linear)
			CachedMoveComp->SetForceMovementByLinear_Local( CachedStartLoc, Duration );
		else if(ForceMoveType == ER4ForceMoveType::CurveVector)
			CachedMoveComp->SetForceMovementByCurve_Local( CachedStartLoc, Duration, CurveVector, true );
	}
}

/**
 *  버프 종료 시 Clear하는 로직을 정의
 */
void UR4Buff_ForceMove::Reset()
{
	Super::Reset();

	CachedStartLoc = FVector::ZeroVector;
	CachedMoveComp.Reset();
}
