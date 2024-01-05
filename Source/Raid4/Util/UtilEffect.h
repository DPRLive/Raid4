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
	UNiagaraComponent* SpawnNiagaraAtLocation_Local(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, const FVector& InLocation, const FRotator& InRotation, const FVector& InScale, UWorld* InWorld = nullptr);
}