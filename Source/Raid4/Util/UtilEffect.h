#pragma once

class UNiagaraSystem;
class UNiagaraComponent;

/**
* Niagara System을 쉽게 사용하기 위한 Util namespace
*/
namespace UtilEffect
{
	/**
	* Niagara System을 생성한다. ( 로컬에서만, 멀티 X )
	*/
	UNiagaraComponent* SpawnNiagaraAtLocation_Local(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, const FVector& InLocation, const FRotator& InRotation, const FVector& InScale, const UWorld* InWorld = nullptr);

	/**
	* Niagara System을 생성하여 Attach 한다. ( 로컬에서만, 멀티 X )
	*/
	UNiagaraComponent* SpawnNiagaraAttached_Local(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, USceneComponent* InAttachComp, FName InSocketName, const FVector& InLocation, const FRotator& InRotation );
}