// Fill out your copyright notice in the Description page of Project Settings.

#include "R4BuffComponent.h"
#include "R4BuffBase.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4BuffComponent)

UR4BuffComponent::UR4BuffComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UR4BuffComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UR4BuffComponent, AppliedBuffs, COND_OwnerOnly);
}

void UR4BuffComponent::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *	버프를 추가 (서버)
 */
void UR4BuffComponent::Server_AddBuff(AActor* InInstigator, TSubclassOf<UR4BuffBase> InBuffClass, const FR4BuffDesc* InBuffDesc)
{
	if(GetOwnerRole() != ROLE_Authority)
		return;
	
	FBuffAppliedInfo buffInfo;

	// TODO : Set Outer?
	buffInfo.ServerBuffInstance = Cast<UR4BuffBase>(OBJECT_POOL->GetObject(InBuffClass));

	// 버프 인스턴스가 유효하지 않으면 리턴
	if(!IsValid(buffInfo.ServerBuffInstance)) 
		return;
	
	// 버프 적용
	buffInfo.ServerBuffInstance->ApplyBuff(InInstigator, GetOwner(), InBuffDesc);
	
	// 지속시간이 필요하다면
	if(buffInfo.ServerBuffInstance->GetBuffDesc().BuffDurationType != EBuffDurationType::OneShot)
	{
		buffInfo.BuffClass = InBuffClass;
		buffInfo.AppliedServerTime = R4GetServerTimeSeconds(GetWorld());
		buffInfo.BuffModifyDesc = *InBuffDesc;
		
		// 버프를 관리하도록 추가
        AppliedBuffs.Emplace(MoveTemp(buffInfo));

		// 버프가 끝나면 관리 목록에서 제거하도록 등록
		buffInfo.ServerBuffInstance->OnEndBuff().AddWeakLambda(this, [this, instance = buffInfo.ServerBuffInstance]
		{
			_Server_RemoveBuffInfo(instance);

			// 인스턴스 Pool에 반납
			OBJECT_POOL->ReturnPoolObject(instance);
		});
	}
}

/**
 *	인스턴스를 비교하여 버프를 관리 목록에서 제거
 */
bool UR4BuffComponent::_Server_RemoveBuffInfo(UR4BuffBase* InBuffInstance)
{
	if(GetOwnerRole() != ROLE_Authority)
		return false;
        
	for(auto it = AppliedBuffs.CreateIterator(); it; ++it)
	{
		if(it->ServerBuffInstance != InBuffInstance)
			continue;
		
		// delegate 제거
		it->ServerBuffInstance->OnEndBuff().RemoveAll(this);
		
		it.RemoveCurrent();
		return true;
	}

	return false;
}