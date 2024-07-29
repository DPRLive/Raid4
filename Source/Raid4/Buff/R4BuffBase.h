// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4BuffDesc.h"
#include "R4BuffBase.generated.h"

class FTimerHandler;

/**
 * Buff의 Base가 되는 클래스.
 * BuffDesc를 활용하여 기본 변수 변경이 필요한 경우 BP 클래스 추가 없이 확장하여 사용 가능.
 */
UCLASS( Abstract )
class RAID4_API UR4BuffBase : public UObject
{
	GENERATED_BODY()

public:
	UR4BuffBase();

public:
	// 버프를 적용
	virtual void ApplyBuff(AActor* InVictim, const FR4BuffDesc* InBuffDesc = nullptr);
	
	// 버프를 제거. 오버라이드 시 필요한거 하고 마지막에 Super를 호출.
	virtual void RemoveBuff();
	
	// Buff Description을 리턴.
	FORCEINLINE const FR4BuffDesc& GetBuffDesc() const { return BuffDesc; }

	// Buff가 끝났는지 알려주는 delegate 리턴
	FORCEINLINE FSimpleMulticastDelegate& OnEndBuff() { return OnEndBuffDelegate; }

protected:
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의.
	virtual void PreActivate(AActor* InVictim, const FR4BuffDesc* InBuffDesc = nullptr);

	// 버프 적용 해제 전 해야 할 로직
	virtual void PreDeactivate() {}
	
	// 버프가 실제로 할 로직을 정의
	virtual void Activate() {}

	// 버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
	virtual void Deactivate();

private:
	// EBuffMode에 따라서 버프를 실행
	void _ActivateByBuffMode(EBuffMode InBuffMode);
	
	// EBuffDurationType에 따라 버프의 제거 타이밍을 설정
	void _SetBuffRemoveTiming(EBuffDurationType InDurationType);

	// Timer getter, 필요한 경우 생성 시킴
	TSharedPtr<FTimerHandler> _GetTimerHandler();
	
protected:
	// 버프가 끝났다고 알려주는 delegate.
	FSimpleMulticastDelegate OnEndBuffDelegate;
	
	// Buff Description
	// 각각 Buff Class별로 다르게 사용 될 수 있음.
	// 외부에서 호출 시 FR4BuffDesc를 넘겨주지 않으면 이 값을 사용.
	UPROPERTY( EditDefaultsOnly, Category = "BaseBuffInfo", meta = (AllowPrivateAccess = true))
	FR4BuffDesc BuffDesc;

	// 버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 가능하고, 필요한 경우 사용
	UPROPERTY( EditDefaultsOnly, Category = "Deactivate", meta = (AllowPrivateAccess = true))
	uint8 bDeactivate:1;

private:
	// 타이머 핸들
	TSharedPtr<FTimerHandler> TimerHandler;
};
