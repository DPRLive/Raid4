// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h>
#include "R4SkillBase.generated.h"

/**
 * Skill의 Base가 되는 클래스
 * Anim Montage를 실행하여 스킬을 시전.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4SkillBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4SkillBase();

protected:
	virtual void BeginPlay() override;

public:
	// 스킬을 사용
	virtual void ActivateSkill();

	// 스킬이 끝나기 전 Cancel
	virtual void CancelSkill();

	// 스킬 사용 완료
	virtual void CompleteSkill();

	// RPC 테스트
	UFUNCTION( Reliable, Server )
	void RPCtest();
	
};
