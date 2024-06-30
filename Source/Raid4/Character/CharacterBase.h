// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Interface/R4DTDataPushable.h"
#include "../Interface/R4Damageable.h"
#include "../Interface/R4Attackable.h"

#include <GameFramework/Character.h>

#include "CharacterBase.generated.h"

class UR4CharacterRPCComponent;
class UR4AttackComponent;
class UR4SkillComponent;
class UR4StatComponent;

/**
 * (NPC, PlayerCharacter 등) 캐릭터에 베이스가 되는 클래스
 */
UCLASS()
class RAID4_API ACharacterBase : public ACharacter, public IR4DTDataPushable, public IR4Damageable, public IR4Attackable
{
	GENERATED_BODY()

public:
	ACharacterBase(const FObjectInitializer& InObjectInitializer);
	
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;

public:
	// Replicate를 거쳐서 anim을 play
	virtual float PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName) override;

	// Replicate를 거쳐서 anim을 stop
	virtual void StopAnimMontage(UAnimMontage* AnimMontage) override;
public:
	// ~ Begin IR4DTDataPushable (Character의 데이터를 초기화한다. ( By DT_Character))
	virtual void PushDTData(FPriKey InPk) override; 
	// ~ End IR4DTDataPushable

	// ~ Begin IR4Damageable (Damage 수신을 처리한다)
	virtual void ReceiveDamage(float InDamage) override;
	// ~ End IR4Damageable

	// ~ Begin IR4Attackable (Damage 주기 처리)
	virtual void ApplyDamage(const int32 InPk, AActor* InVictim) override;
	// ~end IR4Attackable
	
protected:
	// StatComp와 필요한 초기화를 진행한다
	virtual void InitStatComponent(FPriKey InStatPk);
	
	// 이동 속도를 적용한다.
	virtual void ApplyMovementSpeed(float InBaseMovementSpeed, float InModifierMovementSpeed) const;

	// 캐릭터 죽음 처리
	UFUNCTION()
	virtual void Dead();
	
public:
	// 캐릭터 죽음을 알리는 Delegate
	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnCharacterDead );
	UPROPERTY( BlueprintAssignable, VisibleAnywhere )
	FOnCharacterDead OnCharacterDead;
	
protected:
	// 스탯 기능을 부여해주는 Stat Component
	UPROPERTY( VisibleAnywhere, Category = "Stat", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4StatComponent> StatComp;

	// 스킬 기능을 부여해주는 Skill Component
	UPROPERTY( VisibleAnywhere, Category = "Skill", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4SkillComponent> SkillComp;

	// 공격 기능을 부여해주는 Attack Component, 이 Comp를 사용하여 상대에게 Damage를 입힘
	// UPROPERTY( VisibleAnywhere, Category = "Attack", meta = (AllowPrivateAccess = true) )
	// TObjectPtr<UR4AttackComponent> AttackComp;

	// 여러가지 Character를 위한 RPC 기능을 부여해주는 Component
	UPROPERTY( )
	TObjectPtr<UR4CharacterRPCComponent> RPCComp;
};
