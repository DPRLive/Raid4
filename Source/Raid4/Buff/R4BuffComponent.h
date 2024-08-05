// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4BuffDesc.h"

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
		, ServerBuffInstance(nullptr)
		, BuffModifyDesc(FR4BuffDesc())
		{ }
	
	// 걸린 버프 클래스
	UPROPERTY( VisibleInstanceOnly, Transient )
	TSubclassOf<UR4BuffBase> BuffClass;
	
	// 버프가 걸리기 시작한 시간 (서버 타임)
	UPROPERTY( VisibleInstanceOnly, Transient )
	double AppliedServerTime;

	// 버프의 인스턴스. (서버에서만 존재)
	UPROPERTY( NotReplicated, VisibleInstanceOnly, Transient )
	TObjectPtr<UR4BuffBase> ServerBuffInstance;

	// Buff Desc
	UPROPERTY( VisibleInstanceOnly, Transient )
	FR4BuffDesc BuffModifyDesc;
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
	// 버프를 추가
	void Server_AddBuff(AActor* InInstigator, TSubclassOf<UR4BuffBase> InBuffClass, const FR4BuffDesc* InBuffDesc = nullptr);

private:
	//인스턴스를 비교하여 버프 관리 목록에서 제거
	bool _Server_RemoveBuffInfo(UR4BuffBase* InBuffInstance);

private:
	// 적용된 버프 정보들을 관리. (서버 및 오너)
	UPROPERTY( Replicated, Transient, VisibleInstanceOnly, Category = "Buff" )
	TArray<FBuffAppliedInfo> AppliedBuffs;

};
