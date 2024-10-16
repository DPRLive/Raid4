// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "R4DamageReceiveInterface.generated.h"

struct FR4DamageReceiveInfo;

// 죽음을 알림, Damage 피해 알림 Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnDeadDelegate, AActor*, InDeadActor );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnDamagedDelegate, float, InDamage );

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UR4DamageReceiveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Damage를 입을 수 있는 (공격 당할 수 있는) 객체를 위한 인터페이스
 */
class RAID4_API IR4DamageReceiveInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
 	*  데미지를 입는 함수
 	*  @param InInstigator : 데미지를 가한 가해자 액터
 	*  @param InDamageInfo : 데미지에 관한 정보.
 	*/
	virtual void ReceiveDamage( AActor* InInstigator, const FR4DamageReceiveInfo& InDamageInfo ) = 0;

	// 죽음 알림 delegate return
	virtual FOnDeadDelegate& OnDead() = 0;

	// Damage 피해 알림 Delegate return
	virtual FOnDamagedDelegate& OnDamage() = 0;
};
