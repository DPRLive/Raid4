// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerController.h"

#include <GameFramework/Character.h>
#include <Animation/AnimMontage.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerController)

AR4PlayerController::AR4PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

/**
 *  특정 캐릭터에게 애니메이션 플레이를 명령한다.
 *  @param InCharacter : 애니메이션을 플레이할 캐릭터
 *  @param InAnimMontage : 플레이할 애니메이션 몽타주
 */
void AR4PlayerController::ClientRPC_PlayAnimMontage_Implementation(ACharacter* InCharacter, const TSoftObjectPtr<UAnimMontage>& InAnimMontage)
{
	if(IsValid(InCharacter))
	{
		InCharacter->PlayAnimMontage(InAnimMontage.LoadSynchronous());
	}
}

/**
 *  특정 캐릭터에게 애니메이션 중지를 명령한다.
 *  @param InCharacter : 애니메이션을 플레이할 캐릭터
 *  @param InAnimMontage : 중지할 애니메이션 몽타주
 */
void AR4PlayerController::ClientRPC_StopAnimMontage_Implementation(ACharacter* InCharacter, const TSoftObjectPtr<UAnimMontage>& InAnimMontage)
{
	if(IsValid(InCharacter))
	{
		if(InAnimMontage.IsNull())
		{
			InCharacter->StopAnimMontage();
			return;
		}
		
		InCharacter->StopAnimMontage(InAnimMontage.LoadSynchronous());
	}
}
