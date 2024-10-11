// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4AnimationInterface.generated.h"

class UAnimMontage;
struct FAnimMontageInstance;

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
	/**
 	*  ServerTime을 통한 동기화된 Animation Play를 지원. ( InServerTime 기준으로 보정 )
 	*  <Start ServerTime과의 Delay의 처리 방식>
 	*  Loop Animation의 경우 : Delay 된 StartPos에서 시작
 	*  일반 Animation의 경우 : PlayRate를 보정하여 동일 시점에 끝나도록 보정, ( delay > anim length인 경우 : Skip play )
 	*  @param InAnimMontage : Play할 Anim Montage
 	*  @param InStartSectionName : Play할 Anim Section의 Name
 	*  @param InPlayRate : PlayRate, 현재 음수의 play rate는 처리하지 않음.
 	*  @param InStartServerTime : 이 Animation을 Play한 서버 시작 시간 
 	*/
	virtual void PlayAnimSync( UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate, float InStartServerTime ) = 0;

	// 주어진 Montage의 FAnimMontageInstance를 반환, 현재 해당 Montage가 Play되고 있지 않다면 nullptr return
	// ( 주의 )
	// GetInstanceForMontage( InMontage ) -> Blend Out 상태도 return함
	// GetActiveInstanceForMontage( InMontage ) -> 실제 Active된 경우만 return..
	virtual FAnimMontageInstance* GetActiveInstanceForMontage( const UAnimMontage* InMontage ) const = 0;

	// Montage Instance가 사라지는 타이밍을 반환.
	virtual FOnClearMontageInstance* OnClearMontageInstance() = 0;
};
