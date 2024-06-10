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
	
	// 서버에게 내가 이 파티클을 플레이 했으니 뿌려달라고 서버에 알림
	UFUNCTION( Server, Reliable, Category = "RPC|Effect" )
	void ServerRPC_NotifySpawnNiagaraAtLocation(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, const FVector& InLocation, const FRotator& InRotation, const FVector& InScale, const UWorld* InWorld);
	
	// 특정 Controller에게 파티클 플레이를 명령
	UFUNCTION( Client, Unreliable, Category = "RPC|Effect" )
	void ClientRPC_SpawnNiagaraAtLocation(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, const FVector& InLocation, const FRotator& InRotation, const FVector& InScale, const UWorld* InWorld);
	
	// 서버에게 내가 이 파티클을 플레이 했으니 뿌려달라고 서버에 알림 (Attached)
	UFUNCTION( Server, Reliable, Category = "RPC|Effect" )
	void ServerRPC_NotifySpawnNiagaraAttached(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, USceneComponent* InAttachComp, FName InSocketName, const FVector& InLocation, const FRotator& InRotation);
	
	// 특정 Controller에게 파티클 플레이를 명령 (Attached)
	UFUNCTION( Client, Unreliable, Category = "RPC|Effect" )
	void ClientRPC_SpawnNiagaraAttached(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, USceneComponent* InAttachComp, FName InSocketName, const FVector& InLocation, const FRotator& InRotation);
};
