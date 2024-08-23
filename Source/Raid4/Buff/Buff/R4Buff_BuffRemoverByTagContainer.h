// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"
#include "R4Buff_BuffRemoverByTagContainer.generated.h"

class UR4BuffManageComponent;

/**
 * Buff와 Tag로 쿼리하여 Buff를 강제로 해제하는 버프.
 * Buff Manage Comp에 의존.
 * Deactivate 불가.
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Buff), HideCategories=(Deactivate) )
class RAID4_API UR4Buff_BuffRemoverByTagContainer : public UR4BuffBase
{
	GENERATED_BODY()

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
	// 쿼리에 사용할 태그들을 설정.
	// BuffTag.###(QueryTagContainer) 시,
	// 해당 컨테이너에 포함된 Tag들 중 하나라도 조건을 만족하면 해당 Buff를 삭제.
	UPROPERTY( EditDefaultsOnly, meta = ( Categories = "Buff", AllowPrivateAccess = true ) )
	FGameplayTagContainer QueryTagContainer;

	// 태그들을 어떻게 쿼리할 것인지? (QueryTag가 Target Tag에 어떻게 매칭 되는지?)
	UPROPERTY( EditDefaultsOnly, meta = ( AllowPrivateAccess = true ) )
	EGameplayTagQueryType QueryType;
	
	// Buff Manage Comp를 캐싱
	TWeakObjectPtr<UR4BuffManageComponent> CachedBuffManageComp;
};
