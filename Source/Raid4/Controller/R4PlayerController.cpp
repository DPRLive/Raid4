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
 *  서버에게 내가 이 애니메이션을 플레이 했으니 뿌려달라고 서버에 알린다.
 *  @param InCharacter : 애니메이션을 플레이할 캐릭터
 *  @param InAnimMontage : 플레이할 애니메이션 몽타주
 */
void AR4PlayerController::ServerRPC_NotifyPlayAnimMontage_Implementation(ACharacter* InCharacter, const TSoftObjectPtr<UAnimMontage>& InAnimMontage)
{
	for (AR4PlayerController* playerController : TActorRange<AR4PlayerController>(GetWorld()))
	{
		// 요청한 애 빼고 플레이 시킨다.
		if (playerController && this != playerController)
		{
			playerController->ClientRPC_StopAnimMontage(InCharacter, nullptr);
			playerController->ClientRPC_PlayAnimMontage(InCharacter, InAnimMontage);
		}
	}
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

/**
 *  서버에게 내가 이 파티클을 플레이 했으니 뿌려달라고 서버에 알린다.
 */
void AR4PlayerController::ServerRPC_NotifySpawnNiagaraAtLocation_Implementation(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, const FVector& InLocation, const FRotator& InRotation, const FVector& InScale, const UWorld* InWorld)
{
	for (AR4PlayerController* playerController : TActorRange<AR4PlayerController>(GetWorld()))
	{
		// 요청한 애 빼고 플레이 시킨다.
		if (playerController && this != playerController)
		{
			playerController->ClientRPC_SpawnNiagaraAtLocation(InNiagaraSystem, InLocation, InRotation, InScale, InWorld);
		}
	}
}

/**
 *  특정 Controller에게 파티클 플레이를 명령
 */
void AR4PlayerController::ClientRPC_SpawnNiagaraAtLocation_Implementation(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, const FVector& InLocation, const FRotator& InRotation, const FVector& InScale, const UWorld* InWorld)
{
	UtilEffect::SpawnNiagaraAtLocation_Local(InNiagaraSystem, InLocation, InRotation, InScale, InWorld);
}

/**
 *  서버에게 내가 이 파티클을 플레이 했으니 뿌려달라고 서버에 알림 (Attached)
 */
void AR4PlayerController::ServerRPC_NotifySpawnNiagaraAttached_Implementation(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, USceneComponent* InAttachComp, FName InSocketName, const FVector& InLocation, const FRotator& InRotation)
{
	for (AR4PlayerController* playerController : TActorRange<AR4PlayerController>(GetWorld()))
	{
		// 요청한 애 빼고 플레이 시킨다.
		if (playerController && this != playerController)
		{
			playerController->ClientRPC_SpawnNiagaraAttached(InNiagaraSystem, InAttachComp, InSocketName, InLocation, InRotation);
		}
	}
}

/**
 *  특정 Controller에게 파티클 플레이를 명령 (Attached)
 */
void AR4PlayerController::ClientRPC_SpawnNiagaraAttached_Implementation(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, USceneComponent* InAttachComp, FName InSocketName, const FVector& InLocation, const FRotator& InRotation)
{
	UtilEffect::SpawnNiagaraAttached_Local(InNiagaraSystem, InAttachComp, InSocketName, InLocation, InRotation);
}