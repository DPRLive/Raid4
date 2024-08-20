// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"

#include <GameplayTagContainer.h>

#include "R4Buff_StatModifier.generated.h"

class UR4StatBaseComponent;

/**
 * Stat Modifier를 변경할 수 있는 버프.
 * BuffDesc의 Value를 Modifier Stat와의 피연산자로 사용.
 * Stat Comp에 의존.
 * Deactivate 시 변경했던 Modifier의 Delta만큼 원래대로 복구
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Buff) )
class RAID4_API UR4Buff_StatModifier : public UR4BuffBase
{
	GENERATED_BODY()

public:
	UR4Buff_StatModifier();

#if WITH_EDITOR
	virtual bool CanEditChange( const FProperty* InProperty ) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의. 세팅 실패 시 false를 꼭 리턴
	virtual bool PreActivate(AActor* InInstigator, AActor* InVictim, const FR4BuffDesc* InBuffDesc) override;
	
	// 버프가 실제로 할 로직을 정의
	virtual void Activate() override;

	// 버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
	virtual void Deactivate() override;

	// 해당 버프 클래스를 초기 상태로 Reset
	virtual void Reset() override;

private:
	// 무슨 스탯을 변경할 것인지 태그로 설정, Modifier Stat에 적용.
	UPROPERTY( EditDefaultsOnly, meta = (Categories = "Stat", AllowPrivateAccess = true))
	FGameplayTag TargetStatTag;

	// 어떤 Modifier와 연산을 진행할 것인지?
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	EOperatorType ModifierType;

	// Modifier 변경 시, Current Stat이 존재한다면 비례하여 같이 조정할 것인지?
	// ex) 최대 체력이 100에서 150으로 50% 증가 시 현재 체력도 50% 증가. 예를 들어, 기존 체력이 50이면 새로운 체력은 75.
	UPROPERTY( EditDefaultsOnly, Category = "CurrentStat", meta= (AllowPrivateAccess = true))
	uint8 bApplyProportionalAdjustment:1;
	
	// Base Stat Comp 캐싱
	TWeakObjectPtr<UR4StatBaseComponent> CachedStatComp;
	
	// Modifier에 적용한 delta 값
	float CachedDeltaValue;
};
