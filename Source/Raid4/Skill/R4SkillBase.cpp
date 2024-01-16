// Fill out your copyright notice in the Description page of Project Settings.


#include "R4SkillBase.h"

UR4SkillBase::UR4SkillBase()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UR4SkillBase::BeginPlay()
{
	Super::BeginPlay();
}

void UR4SkillBase::ActivateSkill()
{
	RPCtest();
}

void UR4SkillBase::CancelSkill()
{
}

void UR4SkillBase::CompleteSkill()
{
}

void UR4SkillBase::RPCtest_Implementation()
{
	LOG_WARN(LogTemp, TEXT("스킬 RPC"));
}

