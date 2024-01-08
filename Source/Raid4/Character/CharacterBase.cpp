// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "../Component/R4SkillManageComponent.h"
#include "../Component/R4StatManageComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterBase)

ACharacterBase::ACharacterBase(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	SkillManageComp = CreateDefaultSubobject<UR4SkillManageComponent>(TEXT("SkillManageComp"));

	StatManageComp = CreateDefaultSubobject<UR4StatManageComponent>(TEXT("StatManageComp"));
}

/**
 *  begin play
 */
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  현재 캐릭터가 스킬을 사용할 수 있는지 판단
 *  TODO : 추후 디버프, 상태이상 등을 확인
 */
bool ACharacterBase::CanActivateSkill()
{
	return true;
}