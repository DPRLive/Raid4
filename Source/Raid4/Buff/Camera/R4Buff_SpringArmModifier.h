// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"
#include "R4Buff_SpringArmModifier.generated.h"

class UR4CameraManageComponent;

/**
 * Spring Arm 길이를 변경하는 버프.
 * UR4CameraManageComponent에 의존.
 * Deactivate 시 해당 버프가 이동 시킨 만큼 다시 원래대로 이동
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Buff) )
class RAID4_API UR4Buff_SpringArmModifier : public UR4BuffBase
{
	GENERATED_BODY()

	UR4Buff_SpringArmModifier();
	
public:
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의. 세팅( 버프 효과 적용이 가능한 상태인가 ) 실패 시 false를 꼭 리턴,
	// 클래스 상속 시 추가 정보가 필요하다면 오버라이드 하여 세팅 작업을 추가적으로 진행
	virtual bool SetupBuff(AActor* InInstigator, AActor* InVictim) override;
	
	// 버프를 적용, 실제로 해당 Buff가 적용할 효과 로직을 정의. SetupBuff 후 사용.
	virtual bool ApplyBuff() override;

protected:
	// 버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
	virtual void Deactivate() override;

	// 해당 버프 클래스를 초기 상태로 Reset
	virtual void Reset() override;

private:
	// 변화시킬 Delta 길이 값
	UPROPERTY( EditDefaultsOnly )
	float DeltaLength;

	// 이동에 보간 속도. <= 0일 시 즉시 이동.
	UPROPERTY( EditDefaultsOnly )
	float Speed;
	
	// Camera Manage Comp 캐싱
	TWeakObjectPtr<UR4CameraManageComponent> CachedCameraManageComp;
};
