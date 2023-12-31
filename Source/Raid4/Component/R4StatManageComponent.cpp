// Fill out your copyright notice in the Description page of Project Settings.


#include "R4StatManageComponent.h"

#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4StatManageComponent)

UR4StatManageComponent::UR4StatManageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SharedStat = FSharedStat();
	BaseStat = FStatRow();
	ModifierStat = FStatRow();
}

/**
 *	컴포넌트 초기화
 */
void UR4StatManageComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetIsReplicated(true);
}

/**
 *  begin play
 */
void UR4StatManageComponent::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  Replicate 설정
 */
void UR4StatManageComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UR4StatManageComponent, SharedStat)

	DOREPLIFETIME_CONDITION(UR4StatManageComponent, BaseStat, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UR4StatManageComponent, ModifierStat, COND_OwnerOnly);
}

/**
 *  Stat DT의 PK로 Base 스탯을 설정 한다. (서버)
 */
void UR4StatManageComponent::Server_SetBaseStat(const FPriKey& InPK)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;

	const FStatRowPtr statRowPtr(InPK);
	if (!statRowPtr.IsValid())
	{
		LOG_WARN(R4Data, TEXT("PK [%d] is invalid."), InPK);
		return;
	}

	BaseStat = *statRowPtr;
	
	_OnRep_Stat(); // 서버에선 OnRep이 호출이 안되므로 직접 호출

	_Server_UpdateSharedStat();
}

/**
 *  Modifier Stat을 더한다. (서버)
 */
void UR4StatManageComponent::Server_AddModifierStat(const FStatRow& InModifierStat)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;

	ModifierStat += InModifierStat;
	
	_OnRep_Stat(); // 서버에선 OnRep이 호출이 안되므로 직접 호출

	_Server_UpdateSharedStat();
}

/**
 *  Stat을 초기화, SetBaseStat 및 AddModifierStat을 할 필요가 있다면 먼저 하고 난 뒤 호출! (서버)
 */
void UR4StatManageComponent::Server_ResetStat()
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;
	
	SharedStat.NowHp = SharedStat.MaxHp;
	SharedStat.NowMp = SharedStat.MaxMp;

	_OnRep_SharedStat();
}

/**
 *  Now Hp 변경 함수 (서버)
 */
void UR4StatManageComponent::Server_AddDeltaHp(const float InDeltaHp)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;
	
	SharedStat.NowHp = FMath::Clamp(SharedStat.NowHp + InDeltaHp, 0.f, SharedStat.MaxHp);
	_OnRep_SharedStat();
}

/**
 *  Now Mp 변경 함수 (서버)
 */
void UR4StatManageComponent::Server_AddDeltaMp(const float InDeltaMp)
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;

	SharedStat.NowMp = FMath::Clamp(SharedStat.NowMp + InDeltaMp, 0.f, SharedStat.MaxMp);
	_OnRep_SharedStat();
}

/**
 *  모든 유저와 공유해야할 데이터가 Replicate (변경) 되면, 알림
 */
void UR4StatManageComponent::_OnRep_SharedStat() const
{
	LOG_N( R4Data, TEXT("On Change Shared Stat!") );

	if(OnChangeSharedStat.IsBound())
		OnChangeSharedStat.Broadcast(SharedStat);
}

/**
 *  서버와 소유 클라이언트가 공유해야 할 Stat이 변경되면 알림
 */
void UR4StatManageComponent::_OnRep_Stat() const
{
	LOG_N( R4Data, TEXT("On Change Stat!") );
	
	if(OnChangeBaseStat.IsBound())
		OnChangeBaseStat.Broadcast(BaseStat, ModifierStat);
}

/**
 *  BaseStat과 Modifier Stat을 기반으로 Shared Stat을 업데이트 (서버)
 */
void UR4StatManageComponent::_Server_UpdateSharedStat()
{
	if(!ensureMsgf(GetOwnerRole() == ROLE_Authority, TEXT("This func must called by server.")))
		return;
	
	SharedStat.MaxHp = BaseStat.MaxHp + ModifierStat.MaxHp;
	SharedStat.MaxMp = BaseStat.MaxMp + ModifierStat.MaxMp;
	_OnRep_SharedStat();
}
