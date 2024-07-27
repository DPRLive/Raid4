// Fill out your copyright notice in the Description page of Project Settings.


#include "R4BuffBase.h"
#include "R4BuffModifyDesc.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4BuffBase)

UR4BuffBase::UR4BuffBase()
{
	BaseDuration = 0.f;
	CachedDuration = 0.f;
}

void UR4BuffBase::ApplyBuff(AActor* InVictim, const FR4BuffModifyDesc& InModifyDesc)
{
	CachedDuration = BaseDuration * InModifyDesc.TimeFactor;
}
