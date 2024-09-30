// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4BuffStruct.h"

#include <Components/ActorComponent.h>

#include "R4BuffManageComponent.generated.h"

class UR4BuffBase;

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
	void AddBuff( AActor* InInstigator, const TSubclassOf<UR4BuffBase>& InBuffClass, const FR4BuffSettingDesc& InBuffSettingDesc );

	// 무시할 버프의 태그를 추가
	void AddBlockingBuffTag( const FGameplayTag& InTag, EGameplayTagQueryType InQueryType );

	// 무시할 버프의 태그들을 추가
	void AddBlockingBuffTags( const FGameplayTagContainer& InTagContainer, EGameplayTagQueryType InQueryType );

	// 태그로 해당하는 버프를 모두 제거
	void RemoveBuffAllByTag( const FGameplayTag& InTagToQuery, EGameplayTagQueryType InQueryType );

	// 태그 컨테이너로 해당하는 버프를 모두 제거
	void RemoveBuffAllByTags( const FGameplayTagContainer& InTagContainerToQuery, EGameplayTagQueryType InQueryType );

	// 무시할 버프로 관리하던 태그를 제거
	void RemoveBlockingBuffTag( const FGameplayTag& InTag, EGameplayTagQueryType InQueryType );

	// 무시할 버프로 관리하던 태그들을 제거
	void RemoveBlockingBuffTags( const FGameplayTagContainer& InTagContainer, EGameplayTagQueryType InQueryType );
	
private:
	// 버프 세팅에 맞춰 버프를 등록
	void _RegisterBuffBySetting( FAppliedBuffInfo&& InBuffAppliedInfo );

	// 버프를 업데이트.
	bool _UpdateBuffs( float InNowServerTime );

	// Buff Array에 대한 Remove All By Predicate, 인스턴스가 유효하지 않으면 자동으로 제거
	void _RemoveBuffAllByPredicate( TArray<FAppliedBuffInfo>& InBuffArray, const TFunction<bool( FAppliedBuffInfo& )>& InPredicate );

	// Server Buffs Array에서 특정 버프 정보 제거 (Ptr로 비교)
	void _Server_RemoveServerBuffInfo( UR4BuffBase* InBuffInstance );
	
private:
	// 업데이트가 필요한, 적용된 버프 정보들을 관리. 순서 유지를 보장하지 않음.
	UPROPERTY( Transient, VisibleInstanceOnly, Category = "Buff" )
	TArray<FAppliedBuffInfo> UpdatingBuffs;

	// 업데이트가 필요없는, 적용된 버프 정보들을 관리. 순서 유지를 보장하지 않음.
	UPROPERTY( Transient, VisibleInstanceOnly, Category = "Buff" )
	TArray<FAppliedBuffInfo> NonUpdatingBuffs;

	// 무시할 버프가 있다면, 무시할 버프의 태그를 등록. (부모로 사용되어도 일치)
	UPROPERTY( Transient, VisibleInstanceOnly, Category = "Buff" )
	FGameplayTagContainer BlockingBuffTags_Match;

	// 무시할 버프가 있다면, 무시할 버프의 태그를 등록. (완벽히 일치)
	UPROPERTY( Transient, VisibleInstanceOnly, Category = "Buff" )
	FGameplayTagContainer BlockingBuffTags_MatchExact;

	// 서버에서 적용되어 있는 버프들의 정보, 순서 유지를 보장하지 않음.
	// (서버에서 버프 적용 시 오너에게 정보를 알리는 용도로 사용됨)
	// (정보 확인용으로만 사용할 것)
	UPROPERTY( Replicated, Transient, VisibleInstanceOnly, Category = "Buff" )
	TArray<FServerAppliedBuffInfo> ServerBuffs;
};
