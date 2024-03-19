// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4SkillBase.generated.h"

class FCoolTimeHandler;
class UAnimMontage;
/**
 * Skill의 Base가 되는 클래스
 * Anim Montage를 실행하여 스킬을 시전.
 * TODO : 할게 많음.
 */
UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4SkillBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4SkillBase();

protected:
	virtual void BeginPlay() override;

public:
	// 스킬을 준비
	virtual void PrepareSkill();
	
	// 스킬이 끝나기 전 Cancel
	virtual void CancelSkill();

	// 스킬 사용 완료
	virtual void CompleteSkill();

	// 스킬을 사용
	virtual void ActivateSkill();
protected:
	// 서버로 스킬을 사용 했음을 알린다.
	UFUNCTION( Server, Reliable, WithValidation )
	void ServerRPC_ActivateSkill(float InActivateTime);

protected:
	// 스킬의 메인 애니메이션 Montage ( 해당 애니메이션을 플레이하여 스킬을 발동 )
	UPROPERTY( EditAnywhere, Category = "Anim" )
	TSoftObjectPtr<UAnimMontage> SkillAnim;

private:
	// 스킬 쿨타임을 위한 CoolTimeHandler
	//TUniquePtr<FCoolTimeHandler> CoolTimeHandler;
	
	// 마지막으로 발동한 시간 (서버)
	float CachedLastActivateTime;
};
