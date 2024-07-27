// Fill out your copyright notice in the Description page of Project Settings.

#include "R4BuffComponent.h"
#include "R4BuffBase.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4BuffComponent)

UR4BuffComponent::UR4BuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	
	// 적당히 갱신시간 타협
	SetComponentTickInterval(Buff::G_BuffTickInterval);
}

void UR4BuffComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UR4BuffComponent, AppliedBuffs, COND_OwnerOnly);
}

void UR4BuffComponent::BeginPlay()
{
	Super::BeginPlay();

	// Server 에서만 Tick
	if(GetOwnerRole() != ROLE_Authority)
		SetComponentTickEnabled(false);
}

void UR4BuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 버프를 업데이트
	_Server_UpdateBuffs(DeltaTime);
}

/**
 *	버프를 추가 (서버)
 */
void UR4BuffComponent::Server_AddBuff(TSubclassOf<UR4BuffBase> InBuffClass, const FR4BuffModifyDesc& InModifyDesc)
{
	if(GetOwnerRole() != ROLE_Authority)
		return;
	
	FBuffAppliedInfo buffInfo;

	buffInfo.ServerBuffInstance = NewObject<UR4BuffBase>(this, InBuffClass);

	// 버프 인스턴스가 유효하지 않으면 리턴
	if(!IsValid(buffInfo.ServerBuffInstance)) 
		return;
	
	// 버프 적용
	buffInfo.ServerBuffInstance->ApplyBuff(GetOwner(), InModifyDesc);
	
	buffInfo.BuffClass = InBuffClass;
	buffInfo.AppliedServerTime = R4GetServerTimeSeconds(GetWorld());
	buffInfo.EndServerTime = buffInfo.AppliedServerTime + buffInfo.ServerBuffInstance->GetDuration();
	buffInfo.BuffModifyDesc = InModifyDesc;
	
	// 지속시간이 0이 아니면
	if(!FMath::IsNearlyEqual(0.f, buffInfo.ServerBuffInstance->GetDuration()))
	{
		// 버프를 관리하도록 추가
        AppliedBuffs.Emplace(MoveTemp(buffInfo));
	}
}

/**
 *	버프들을 업데이트
 */
void UR4BuffComponent::_Server_UpdateBuffs(float InDeltaTime)
{
	if(GetOwnerRole() != ROLE_Authority)
		return;
		
	double serverTime = R4GetServerTimeSeconds(GetWorld());
	
	for(auto it = AppliedBuffs.CreateIterator(); it; ++it)
	{
		// 끝날 시간이 아니면 continue
		if(it->EndServerTime > serverTime)
			continue;
		
		// 버프 제거 로직 후
		if(IsValid(it->ServerBuffInstance))
			it->ServerBuffInstance->RemoveBuff(GetOwner());
		
		// 관리 목록에서 제거
		it.RemoveCurrent();
	}
}
