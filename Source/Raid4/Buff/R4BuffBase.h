// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/NoExportTypes.h>
#include "R4BuffBase.generated.h"

struct FR4BuffModifyDesc;

/**
 * Buff의 Base가 되는 클래스.
 * 기본 값 * factor (BuffModifyDesc)으로 실행
 * factor의 적용 방식은 버프마다 다를 수 있음.
 */
UCLASS( Abstract )
class RAID4_API UR4BuffBase : public UObject
{
	GENERATED_BODY()

public:
	UR4BuffBase();
	
	// 버프 적용 시 행동
	virtual void ApplyBuff(AActor* InVictim, const FR4BuffModifyDesc& InModifyDesc);

	// 버프 제거 시 행동
	virtual void RemoveBuff(AActor* InVictim) PURE_VIRTUAL(UR4BuffBase::RemoveBuff, );

	// 기본 지속 시간을 리턴.
	FORCEINLINE float GetBaseDuration() const { return BaseDuration; }

	// TimeFactor가 적용된 지속 시간을 리턴.
	FORCEINLINE float GetDuration() const { return CachedDuration; }

private:
	// 기본 지속 시간, FR4BuffModifyDesc의 Time Factor가 여기에 곱해짐 ! 
	UPROPERTY( EditDefaultsOnly, Category = "BuffInfo", meta = (AllowPrivateAccess = true))
	float BaseDuration;

	// cached
	// TimeFactor가 적용된 지속 시간.
	float CachedDuration;
};
