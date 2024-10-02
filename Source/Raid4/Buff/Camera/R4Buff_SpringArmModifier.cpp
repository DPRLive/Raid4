// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Buff_SpringArmModifier.h"

#include "../../Camera/R4CameraManageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Buff_SpringArmModifier)

UR4Buff_SpringArmModifier::UR4Buff_SpringArmModifier()
{
	bDeactivate = true;

	DeltaLength = 0.f;
	Speed = 0.f;
}

/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_SpringArmModifier::SetupBuff(AActor* InInstigator, AActor* InVictim)
{
	bool bReady = Super::SetupBuff(InInstigator, InVictim);

	// 버프 받을 객체의 Camera Comp를 캐싱
	if ( CachedVictim.IsValid() )
		CachedCameraManageComp = CachedVictim->FindComponentByClass<UR4CameraManageComponent>();

	return bReady && CachedCameraManageComp.IsValid();
}

/**
 *  버프를 적용 ( Camera Length Delta를 적용 )
 */
bool UR4Buff_SpringArmModifier::ApplyBuff()
{
	if ( !Super::ApplyBuff() )
		return false;

	if ( CachedCameraManageComp.IsValid() && CachedVictim.IsValid() )
	{
		CachedCameraManageComp->AddSpringArmLength( DeltaLength );
		CachedCameraManageComp->SetSpringArmResizeSpeed( Speed );
		return true;
	}

	return false;
}

/**
 *  버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
 *  Deactivate 시 해당 버프가 이동 시킨 만큼 다시 원래대로 이동
 */
void UR4Buff_SpringArmModifier::Deactivate()
{
	Super::Deactivate();

	// 원래대로 이동
	if ( CachedCameraManageComp.IsValid() && CachedVictim.IsValid() )
	{
		CachedCameraManageComp->AddSpringArmLength( - DeltaLength );
		CachedCameraManageComp->SetSpringArmResizeSpeed( Speed );
	}
}

/**
 *  버프 종료 시 Clear하는 로직을 정의
 */
void UR4Buff_SpringArmModifier::Reset()
{
	Super::Reset();

	CachedCameraManageComp.Reset();
}
