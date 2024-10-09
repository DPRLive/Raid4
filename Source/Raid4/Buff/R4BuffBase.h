// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Core/ObjectPool/PoolableInterface.h"
#include "R4BuffBase.generated.h"

/**
 * Buff의 Base가 되는 클래스.
 * 버프는 게임내의 단순한 버프 뿐만 아니라 특별한 상황을 처리.
 * 상속 후 해당 버프가 어떤 효과를 줄 것인지 로직 작성.
 * BP 클래스를 확장하여 다양한 버프를 제작
 */
UCLASS( Abstract )
class RAID4_API UR4BuffBase : public UObject, public IPoolableInterface
{
	GENERATED_BODY()

public:
	UR4BuffBase();

public:
	// ~ Begin IPoolableInterface
	// 파생 클래스가 딱히 신경 쓸 게 없음.
	virtual void PostInitPoolObject() override final {}
	virtual void PreReturnPoolObject() override final;
	// ~ End IPoolableInterface
	
	// Getter
	FORCEINLINE AActor* GetInstigator() const { return CachedInstigator.Get(); }
	FORCEINLINE AActor* GetVictim() const { return CachedVictim.Get(); }
	FORCEINLINE const FGameplayTag& GetBuffTag() const { return BuffTag; }
	
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의. 세팅( 버프 효과 적용이 가능한 상태인가 ) 실패 시 false를 꼭 리턴,
	// 클래스 상속 시 추가 정보가 필요하다면 오버라이드 하여 세팅 작업을 추가적으로 진행
	virtual bool SetupBuff(AActor* InInstigator, AActor* InVictim);
	
	// 버프를 적용, 실제로 해당 Buff가 적용할 효과 로직을 정의. SetupBuff 후 사용.
	virtual bool ApplyBuff() { return true; }
	
	// 버프를 제거 시 해야할 로직을 정의.
	virtual void RemoveBuff();

protected:
	// 버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
	virtual void Deactivate() {}

	// 해당 버프 클래스를 초기 상태로 Reset
	virtual void Reset();
	
protected:
	// 버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 가능하고, 필요한 경우 사용
	UPROPERTY( EditDefaultsOnly, Category = "Deactivate" )
	uint8 bDeactivate:1;

	// Buff를 Tag로 식별하고자 하는 경우 Tag 설정
	UPROPERTY( EditDefaultsOnly, Category = "Tag", meta = ( Categories = "Buff" ) )
	FGameplayTag BuffTag;
	
	// 시전자가 누군지 캐싱
	TWeakObjectPtr<AActor> CachedInstigator;

	// 버프를 받은 대상을 캐싱
	TWeakObjectPtr<AActor> CachedVictim;
};
