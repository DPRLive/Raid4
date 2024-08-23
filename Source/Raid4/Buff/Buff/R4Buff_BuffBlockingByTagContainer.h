// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../R4BuffBase.h"
#include "R4Buff_BuffBlockingByTagContainer.generated.h"

class UR4BuffManageComponent;

/**
 * Buff가 Tag를 기반으로 버프를 무시하게 해줄 수 있는 버프 ( Buff Manage Comp의 Blocking Tag에 Tag를 추가 )
 * Buff Manage Comp에 의존.
 * Deactivate 시 적용했던 Blocking Tag를 삭제.
 */
UCLASS( HideDropdown, NotBlueprintType, Blueprintable, ClassGroup=(Buff) )
class RAID4_API UR4Buff_BuffBlockingByTagContainer : public UR4BuffBase
{
	GENERATED_BODY()

public:
	UR4Buff_BuffBlockingByTagContainer();
	
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
	// Blocking에 사용할 태그들을 설정.
	// BuffTag.###(BlockingTagContainer) 시,
	// 해당 컨테이너에 포함된 Tag들 중 하나라도 조건을 만족하면 해당 Buff를 Blocking
	UPROPERTY( EditDefaultsOnly, meta = ( Categories = "Buff", AllowPrivateAccess = true ) )
	FGameplayTagContainer BlockingTagContainer;

	// 태그들을 어떻게 쿼리할 것인지? (BlockingTag가 Target Tag에 어떻게 매칭 되는지?)
	UPROPERTY( EditDefaultsOnly, meta = ( AllowPrivateAccess = true ) )
	EGameplayTagQueryType QueryType;
	
	// Buff Manage Comp를 캐싱
	TWeakObjectPtr<UR4BuffManageComponent> CachedBuffManageComp;
};
