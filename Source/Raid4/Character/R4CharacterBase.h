// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Data/R4DTDataPushInterface.h"
#include "../Damage/R4DamageReceiveInterface.h"
#include "../UI/StatusBar/R4StatusBarInterface.h"
#include "../Buff/R4BuffReceiveInterface.h"
#include "../Animation/R4AnimationInterface.h"
#include "../Stat/R4TagStatQueryInterface.h"

#include <GameFramework/Character.h>

#include "Raid4/Buff/R4BuffStruct.h"
#include "R4CharacterBase.generated.h"

class UR4ShieldComponent;
class UR4CharacterStatComponent;
class UR4SkillComponent;
class UR4BuffManageComponent;
class UR4AnimationComponent;

// TEST
USTRUCT(BlueprintType)
struct FBuffTest
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere )
	TSubclassOf<UR4BuffBase> BuffClass;

	UPROPERTY( EditAnywhere )
	FR4BuffSettingDesc BuffDesc;
};

/**
 * (NPC, PlayerCharacter 등) 캐릭터에 베이스가 되는 클래스
 */
UCLASS()
class RAID4_API AR4CharacterBase : public ACharacter, public IR4DTDataPushInterface,
									public IR4DamageReceiveInterface, public IR4BuffReceiveInterface,
									public IR4TagStatQueryInterface,
									public IR4StatusBarInterface, public IR4AnimationInterface
{
	GENERATED_BODY()

public:
	AR4CharacterBase(const FObjectInitializer& InObjectInitializer);
	
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere)
	TArray<FBuffTest> TestingBuffs;
protected:
	virtual void BeginPlay() override;

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// ~ Begin IR4AnimationInterface
	virtual void PlayAnimSync( UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate, float InStartServerTime ) override;
	virtual FAnimMontageInstance* GetActiveInstanceForMontage( const UAnimMontage* InMontage ) const override;
	virtual FOnClearMontageInstance* OnClearMontageInstance() override;
	// ~ End IR4AnimationInterface

	// ~ Begin IR4DTDataPushable (Character의 데이터를 초기화한다. ( By DT_Character))
	virtual void PushDTData(FPriKey InPk) override; 
	// ~ End IR4DTDataPushable

	// ~ Begin IR4DamageReceiveInterface
	virtual void ReceiveDamage(AActor* InInstigator, const FR4DamageReceiveInfo& InDamageInfo) override;
	// ~ End IR4DamageReceiveInterface

	// ~ Begin IR4StatusBarInterface
	virtual void SetupStatusBarWidget(UUserWidget* InWidget) override;
	// ~ End IR4StatusBarInterface
	
	// ~ Begin IR4BuffReceiveInterface
	virtual void ReceiveBuff(AActor* InInstigator, const TSubclassOf<UR4BuffBase>& InBuffClass, const FR4BuffSettingDesc& InBuffSettingDesc) override;
	// ~ End IR4BuffReceiveInterface

	// ~ Begin IR4TagStatQueryInterface
	virtual FR4StatInfo* GetStatByTag( const FGameplayTag& InTag ) const override;
	virtual FR4CurrentStatInfo* GetCurrentStatByTag( const FGameplayTag& InTag ) const override;
	// ~ End IR4TagStatQueryInterface

	// 죽었는지 반환
	bool IsDead() const { return bDead; }
	
protected:
	// StatComp와 필요한 바인딩을 진행
	virtual void BindStatComponent();
	
	// 이동 속도를 적용한다.
	virtual void ApplyMovementSpeed(float InMovementSpeed) const;

	// 캐릭터 죽음 처리
	UFUNCTION()
	virtual void Dead();
	
public:
	// 캐릭터 죽음을 알리는 Delegate
	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnCharacterDeadDelegate );
	UPROPERTY( BlueprintAssignable, VisibleAnywhere )
	FOnCharacterDeadDelegate OnCharacterDeadDelegate;

	// Damage 피해 알림
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FOnCharacterDamaged, const AActor*, InInstigator, float, InDamage );
	UPROPERTY( BlueprintAssignable, VisibleAnywhere )
	FOnCharacterDamaged OnCharacterDamagedDelegate;
protected:
	// 스탯 기능을 부여해주는 Stat Component
	UPROPERTY( VisibleAnywhere, Category = "Stat" )
	TObjectPtr<UR4CharacterStatComponent> StatComp;

	// 스킬 기능을 부여해주는 Skill Component
	UPROPERTY( VisibleAnywhere, Category = "Skill" )
	TObjectPtr<UR4SkillComponent> SkillComp;

	// 버프 관리 기능을 부여해주는 Buff Component
	UPROPERTY( VisibleAnywhere, Category = "Buff" )
	TObjectPtr<UR4BuffManageComponent> BuffManageComp;

	// 방어막 기능을 부여해주는 Shield Component
	UPROPERTY( VisibleAnywhere, Category = "Shield" )
	TObjectPtr<UR4ShieldComponent> ShieldComp;
	
	// 여러가지 Animation 기능을 위한 Component
	UPROPERTY( VisibleAnywhere, Category = "Anim" )
	TObjectPtr<UR4AnimationComponent> AnimComp;

private:
	// 죽은 상태인지 여부.
	uint8 bDead:1;
};
