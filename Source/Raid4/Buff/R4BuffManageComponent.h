// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4BuffStruct.h"

#include <Components/ActorComponent.h>

#include "R4BuffManageComponent.generated.h"

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
		, BuffSettingDesc(FR4BuffSettingDesc())
		, FirstAppliedServerTime(0.f)
		, LastAppliedServerTime(0.f)
		, ServerBuffInstance(nullptr)
		{ }
	
	// 걸린 버프 클래스
	UPROPERTY( VisibleInstanceOnly, Transient )
	TSubclassOf<UR4BuffBase> BuffClass;

	// 버프 세팅 정보
	UPROPERTY( VisibleInstanceOnly, Transient )
	FR4BuffSettingDesc BuffSettingDesc;
	
	// 버프가 걸리기 시작한 시간 (서버 타임)
	UPROPERTY( VisibleInstanceOnly, Transient )
	double FirstAppliedServerTime;

	// 버프 효과가 마지막에 발동한 시간 (서버 타임)
	UPROPERTY( VisibleInstanceOnly, Transient )
	double LastAppliedServerTime;
	
	// 버프의 인스턴스. (서버에서만 존재)
	UPROPERTY( NotReplicated, VisibleInstanceOnly, Transient )
	TObjectPtr<UR4BuffBase> ServerBuffInstance;
};

/**
 * 객체에 적용되는 '버프'를 관리하는 컴포넌트.
 */
UCLASS( ClassGroup=(Buff), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4BuffManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4BuffManageComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	
public:    
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:	
	// 버프를 추가
	void Server_AddBuff(AActor* InInstigator, const TSubclassOf<UR4BuffBase>& InBuffClass, const FR4BuffSettingDesc& InBuffSettingDesc);

private:
	// 버프 세팅에 맞춰 버프를 등록
	void _Server_RegisterBuffBySetting(FBuffAppliedInfo&& InBuffAppliedInfo);

	// 버프를 업데이트.
	void _Server_UpdateBuffs();
	
private:
	// 업데이트가 필요한, 적용된 버프 정보들을 관리. (서버 및 오너), 순서 유지를 보장하지 않음.
	UPROPERTY( Replicated, Transient, VisibleInstanceOnly, Category = "Buff" )
	TArray<FBuffAppliedInfo> UpdatingBuffs;

	// 업데이트가 필요없는, 적용된 버프 정보들을 관리. (서버 및 오너), 순서 유지를 보장하지 않음.
	UPROPERTY( Replicated, Transient, VisibleInstanceOnly, Category = "Buff" )
	TArray<FBuffAppliedInfo> NonUpdatingBuffs;
};
