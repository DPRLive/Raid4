// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4SkillComponent.generated.h"

class UR4SkillBase;

/**
 * 스킬 기능을 부여해주는 SkillComponent
 */
UCLASS( ClassGroup=(Skill), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4SkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4SkillComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	

public:
	// 스킬을 추가한다. (서버)
	virtual void Server_AddSkill( uint8 InSkillIndex, UR4SkillBase* InSkill );

	// 관리중이던 Skill들을 모두 해제.
	virtual void ClearSkills();

	// Cooldown getter
	float GetSkillCooldownRemaining( int32 InSkillIndex ) const;

protected:
	// 스킬이 등록 된 후 호출.
	virtual void PostAddSkill( uint8 InSkillIndex, UR4SkillBase* InSkill );

	// 스킬이 등록 해제 되기 전 호출
	virtual void PreRemoveSkill( uint8 InSkillIndex, UR4SkillBase* InSkill ) {}
	
private:
	UFUNCTION()
	virtual void _OnRep_SkillInstances( const TArray<UR4SkillBase*>& InPrev );

public:
	// 특정 Skill Index 에 쿨타임이 적용됨을 알림 .
	DECLARE_MULTICAST_DELEGATE_OneParam( FOnSkillCooldownDelegate, int32 );
	FOnSkillCooldownDelegate OnSkillCooldownDelegate;
	
protected:
	// 인스턴스화된 스킬 배열
	UPROPERTY( ReplicatedUsing = _OnRep_SkillInstances, Transient, VisibleAnywhere )
	TArray<UR4SkillBase*> SkillInstances;
};
