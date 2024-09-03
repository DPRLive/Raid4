// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4AnimationInterface.generated.h"

class UAnimMontage;

// This class does not need to be modified.
UINTERFACE()
class UR4AnimationInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Animation RPC, Animation 동기화를 제공하는 객체를 위한 interface
 */
class RAID4_API IR4AnimationInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Local에서 Anim Play
	virtual float PlayAnim_Local(UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate) = 0;

	// Local에서 Anim Stop
	virtual void StopAnim_Local() = 0;
	
	// Server에서, Autonomous Proxy를 제외하고 AnimPlay를 명령. ServerTime 조정으로 동기화 가능.
	virtual float Server_PlayAnim_WithoutAutonomous(UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate, bool InIsWithServer, float InServerTime = -1) = 0;
	
	// Server에서, Autonomous Proxy를 제외하고 AnimStop을 명령.
	virtual void Server_StopAnim_WithoutAutonomous(bool InIsWithServer) = 0;
};
