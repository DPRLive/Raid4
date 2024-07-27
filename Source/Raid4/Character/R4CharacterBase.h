// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Data/R4DTDataPushInterface.h"
#include "../Damage/R4DamageReceiveInterface.h"
#include "../UI/StatusBar/R4StatusBarInterface.h"
#include "../Stat/R4StatInterface.h"
#include "../Buff/R4BuffReceiveInterface.h"

#include <GameFramework/Character.h>

#include "Raid4/Buff/R4BuffModifyDesc.h"
#include "R4CharacterBase.generated.h"

class UR4CharacterStatComponent;
class UR4SkillComponent;
class UR4BuffComponent;
class UR4DamageControlComponent;
class UR4CharacterRPCComponent;

/**
 * (NPC, PlayerCharacter 등) 캐릭터에 베이스가 되는 클래스
 */
UCLASS()
class RAID4_API AR4CharacterBase : public ACharacter, public IR4DTDataPushInterface,
									public IR4StatInterface, public IR4DamageReceiveInterface,
									public IR4BuffReceiveInterface, public IR4StatusBarInterface
{
	GENERATED_BODY()

public:
	AR4CharacterBase(const FObjectInitializer& InObjectInitializer);
	
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UR4BuffBase> TestingBuff;

	// UPROPERTY(EditDefaultsOnly)
	// FR4BuffModifyDesc TestingDesc;
protected:
	virtual void BeginPlay() override;

public:
	// Replicate를 거쳐서 anim을 play
	virtual float PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName) override;

	// Replicate를 거쳐서 anim을 stop
	virtual void StopAnimMontage(UAnimMontage* AnimMontage) override;

	// ~ Begin IR4DTDataPushable (Character의 데이터를 초기화한다. ( By DT_Character))
	virtual void PushDTData(FPriKey InPk) override; 
	// ~ End IR4DTDataPushable

	// ~ Begin IR4StatInterface
	virtual UR4StatBaseComponent* GetStatComponent() override;
	// ~ End IR4StatInterface
	
	// ~ Begin IR4Damageable
	virtual void ReceiveDamage(AActor* InInstigator, float InDamage) override;
	// ~ End IR4Damageable

	// ~ Begin IR4StatusBarInterface
	virtual void SetupStatusBarWidget(UUserWidget* InWidget) override;
	// ~ End IR4StatusBarInterface
	
	// ~ Begin IR4BuffReceiveInterface
	virtual void ReceiveBuff(TSubclassOf<UR4BuffBase> InBuffClass, const FR4BuffModifyDesc& InModifyDesc) override;
	// ~ End IR4BuffReceiveInterface
protected:
	// StatComp와 필요한 바인딩을 진행
	virtual void BindStatComponent();
	
	// 이동 속도를 적용한다.
	virtual void ApplyMovementSpeed(float InBaseMovementSpeed, float InModifierMovementSpeed) const;

	// 캐릭터 죽음 처리
	UFUNCTION()
	virtual void Dead();
	
public:
	// 캐릭터 죽음을 알리는 Delegate
	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnCharacterDeadDelegate );
	UPROPERTY( BlueprintAssignable, VisibleAnywhere )
	FOnCharacterDeadDelegate OnCharacterDeadDelegate;

protected:
	// 스탯 기능을 부여해주는 Stat Component
	UPROPERTY( VisibleAnywhere, Category = "Stat", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4CharacterStatComponent> StatComp;

	// 스킬 기능을 부여해주는 Skill Component
	UPROPERTY( VisibleAnywhere, Category = "Skill", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4SkillComponent> SkillComp;

	// 버프 기능을 부여해주는 Buff Component
	UPROPERTY( VisibleAnywhere, Category = "Buff", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4BuffComponent> BuffComp;
	
	// 데미지 받을때 계산을 도와주는 Damage Control Comp
	UPROPERTY( VisibleAnywhere, Category = "Damage", meta = (AllowPrivateAccess = true) )
	TObjectPtr<UR4DamageControlComponent> DamageControlComp;
	
	// 여러가지 Character를 위한 RPC 기능을 부여해주는 Component
	UPROPERTY( )
	TObjectPtr<UR4CharacterRPCComponent> RPCComp;
};
