// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Data/DataTable/Row/StatRow.h"
#include <Components/ActorComponent.h>
#include "R4StatManageComponent.generated.h"

/**
 * 다른 유저와 공유해야하는 Stat 데이터, UI에 사용 예정
 */
USTRUCT()
struct FSharedStat
{
	GENERATED_BODY()

	FSharedStat() : MaxHp(0.f), NowHp(0.f), MaxMp(0.f), NowMp(0.f) {}
	
	UPROPERTY()
	float MaxHp;

	UPROPERTY()
	float NowHp;

	UPROPERTY()
	float MaxMp;

	UPROPERTY()
	float NowMp;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeSharedStatDelegate, const FSharedStat& /* InSharedStat */)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChangeStatDelegate, const FStatRow& /* InBaseStat */, const FStatRow& /* InModifierStat */)

/**
 * 객체에 스탯을 부여할 때, 그 스탯을 관리해주는 컴포넌트
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4StatManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4StatManageComponent();

	// 컴포넌트 초기화
	virtual void InitializeComponent() override;

protected:
	// BeginPlay
	virtual void BeginPlay() override;

	// Replicate 설정
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	// Stat DT의 PK로 Base 스탯을 설정 한다. (서버)
	void Server_SetBaseStat(const FPriKey& InPK);

	// Modifier Stat을 더한다. (서버)
	void Server_AddModifierStat(const FStatRow& InModifierStat);

	// Stat을 초기화, SetBaseStat 및 AddModifierStat을 할 필요가 있다면 먼저 하고 난 뒤 호출! (서버)
	void Server_ResetStat();
	
	// Getter
	FORCEINLINE const FSharedStat& GetSharedStat() const { return SharedStat; }
	FORCEINLINE const FStatRow& GetBaseStat() const { return BaseStat; }
	FORCEINLINE const FStatRow& GetModifierStat() const { return ModifierStat; }
	FORCEINLINE FStatRow GetTotalStat() const { return BaseStat + ModifierStat; } 

	// Now Hp 변경 함수 (서버)
	void Server_AddDeltaHp(const float InDeltaHp);

	// Now Mp 변경 함수 (서버)
	void Server_AddDeltaMp(const float InDeltaMp);
	
private:
	// 모든 유저와 공유해야할 데이터가 Replicate (변경) 되면, 알림
	UFUNCTION()
	void _OnRep_SharedStat() const;

	// 서버와 소유 클라이언트가 공유해야 할 Stat이 변경되면 알림
	UFUNCTION()
	void _OnRep_Stat() const;
	
	// BaseStat과 Modifier Stat을 기반으로 Shared Stat을 업데이트 (서버)
	void _Server_UpdateSharedStat();
	
public:
	// 모든 유저와 공유해야할 스탯 변경 알림 Delegate
	FOnChangeSharedStatDelegate OnChangeSharedStat;

	// 서버와 소유 클라이언트만 공유해야 하는 스탯 변경 알림 Delegate
	FOnChangeStatDelegate OnChangeBaseStat;
	
private:
	// 모든 유저와 공유해야할 Stat 데이터
	UPROPERTY( ReplicatedUsing = _OnRep_SharedStat, Transient, VisibleInstanceOnly, Category = "Stat", Meta = (AllowPrivateAccess = true) )
	FSharedStat SharedStat;

	// 객체의 Base Stat, 서버와 소유 클라이언트만 공유
	UPROPERTY( ReplicatedUsing = _OnRep_Stat, Transient, VisibleInstanceOnly, Category = "Stat", Meta = (AllowPrivateAccess = true) )
	FStatRow BaseStat;

	// 객체의 Modifier Stat (버프나 아이템 등에 의한 변경 수치)
	UPROPERTY( ReplicatedUsing = _OnRep_Stat, Transient, VisibleInstanceOnly, Category = "Stat", Meta = (AllowPrivateAccess = true) )
	FStatRow ModifierStat;
};
