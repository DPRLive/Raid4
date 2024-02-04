// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/PlayerController.h>
#include "R4PlayerController.generated.h"

class UAnimMontage;
class ACharacter;
/**
 * Player가 사용할 Player Controller
 */
UCLASS()
class RAID4_API AR4PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AR4PlayerController();

	// 특정 캐릭터에게 애니메이션 몽타주 플레이를 명령
	UFUNCTION( Client, Unreliable, Category = "RPC|Anim" )
	void ClientRPC_PlayAnimMontage(ACharacter* InCharacter, const TSoftObjectPtr<UAnimMontage>& InAnimMontage);

	// 특정 캐릭터에게 애니메이션 몽타주 중지 명령
	UFUNCTION( Client, Unreliable, Category = "RPC|Anim", Unreliable )
	void ClientRPC_StopAnimMontage(ACharacter* InCharacter, const TSoftObjectPtr<UAnimMontage>& InAnimMontage);
};
