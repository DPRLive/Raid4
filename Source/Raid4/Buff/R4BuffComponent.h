// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4BuffModifyDesc.h"

#include <Components/ActorComponent.h>
#include "R4BuffComponent.generated.h"

class UR4BuffBase;

/**
 * '걸린 버프' 정보를 관리.
 */
USTRUCT()
struct FBuffAppliedInfo
{
	GENERATED_BODY()

	FBuffAppliedInfo ()
		: BuffClass(nullptr)
		, AppliedServerTime(0.f)
		, EndServerTime(0.f)
		, ServerBuffInstance(nullptr)
		, BuffModifyDesc(FR4BuffModifyDesc())
		{ }
	
	// 걸린 버프 클래스
	UPROPERTY( VisibleInstanceOnly, Transient )
	TSubclassOf<UR4BuffBase> BuffClass;
	
	// 버프가 걸리기 시작한 시간 (서버 타임)
	UPROPERTY( VisibleInstanceOnly, Transient )
	double AppliedServerTime;

	// 버프가 끝나야 하는 시간 (서버 타임)
	UPROPERTY( VisibleInstanceOnly, Transient )
	double EndServerTime;
	
	// 버프의 인스턴스. (서버에서만 존재)
	UPROPERTY( NotReplicated, VisibleInstanceOnly, Transient )
	TObjectPtr<UR4BuffBase> ServerBuffInstance;

	// 버프의 factor
	UPROPERTY( VisibleInstanceOnly, Transient )
	FR4BuffModifyDesc BuffModifyDesc;
};

/**
 * 객체에 적용되는 '버프'를 관리하는 컴포넌트.
 * 버프는 게임내의 단순한 버프 뿐만 아니라 특별한 상황을 처리.
 */
UCLASS( ClassGroup=(Buff), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4BuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4BuffComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 버프를 추가
	void Server_AddBuff(TSubclassOf<UR4BuffBase> InBuffClass, const FR4BuffModifyDesc& InModifyDesc);

private:
	// 버프들을 업데이트
	void _Server_UpdateBuffs(float InDeltaTime);
	
private:
	// 적용된 버프 정보들을 관리. (서버 및 오너)
	UPROPERTY( Replicated, Transient, VisibleInstanceOnly, Category = "Buff" )
	TArray<FBuffAppliedInfo> AppliedBuffs;

};
