// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4DamageControlComponent.generated.h"

// 새로 입힐 데미지가 들어왔을때를 알릴 delegate
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewDamageDelegate, float);

/**
 * 피해를 입을때 (데미지를 받는 사람 입장에서)의 데미지를 변동시키는 요인이 있는 경우
 * 적용하기 위한 컴포넌트.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4DamageControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4DamageControlComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 입히고 싶은 데미지를 적용
	void PushNewDamage(float InDamage);

	// 데미지를 수정
	void ModifyDamage(float InDeltaDamage);

	// 최종 계산되어 입을 순수 데미지를 얻어옴.
	float GetCalculatedDamage();

public:
	// 새로 데미지가 들어옴을 알림, 수정이 필요한 경우 여기에 bind하여 ModifyDamage를 통해 수정
	FOnNewDamageDelegate OnNewDamageDelegate;
	
private:
	// 데미지 계산을 위해 캐싱
	float CachedDamage;

	// 계산된 데미지를 저장
	float CachedCalculatedDamage;
};
