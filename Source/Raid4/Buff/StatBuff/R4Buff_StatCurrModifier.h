// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"
#include "../../Value/R4ValueSelector.h"

#include "R4Buff_StatCurrModifier.generated.h"

class UR4StatBaseComponent;

/**
 * 'Current' Stat을 변경할 수 있는 버프. ( Current Stat에 사용 )
 * Value를 Current Stat와의 피연산자로 사용.
 * Stat Comp에 의존.
 * Deactivate 불가.
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Buff),  HideCategories=(Deactivate) )
class RAID4_API UR4Buff_StatCurrModifier : public UR4BuffBase
{
	GENERATED_BODY()
	
public:
	UR4Buff_StatCurrModifier();

public:
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의. 세팅( 버프 효과 적용이 가능한 상태인가 ) 실패 시 false를 꼭 리턴,
	// 클래스 상속 시 추가 정보가 필요하다면 오버라이드 하여 세팅 작업을 추가적으로 진행
	virtual bool SetupBuff(AActor* InInstigator, AActor* InVictim) override;
	
	// 버프를 적용, 실제로 해당 Buff가 적용할 효과 로직을 정의. SetupBuff 후 사용.
	virtual bool ApplyBuff() override;

protected:
	// 해당 버프 클래스를 초기 상태로 Reset
	virtual void Reset() override;
	
private:
	// 무슨 스탯을 변경할 것인지 태그로 설정
	UPROPERTY( EditDefaultsOnly, meta = (Categories = "Stat.Current", AllowPrivateAccess = true))
	FGameplayTag TargetStatTag;
	
	// 기존 Current 값과 어떤 연산을 진행할 것인지?
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	EOperatorType OperatorType;

	// 변경할 값, Current Stat와의 피연산자로 사용.
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FR4ValueSelector Value;
	
	// Current Value가 음수로 내려가는 것을 허용할 것인지
	UPROPERTY( EditDefaultsOnly, Category = "Clamp",  meta = (AllowPrivateAccess = true))
	uint8 bAllowNegative:1;

	// Current Value가 Total 스탯 값((Base + AddModify) * MultiplyModify) 을 넘어가는 것을 허용할 것인지
	UPROPERTY( EditDefaultsOnly, Category = "Clamp",  meta = (AllowPrivateAccess = true))
	uint8 bAllowOverTotalStat:1;
	
	// Base Stat Comp 캐싱
	TWeakObjectPtr<UR4StatBaseComponent> CachedStatComp;
};
