#include "UtilEffect.h"

#include <NiagaraSystem.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>

/**
* Niagara System을 생성한다. ( 로컬에서만, 멀티 X )
*/
UNiagaraComponent* UtilEffect::SpawnNiagaraAtLocation_Local(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, const FVector& InLocation, const FRotator& InRotation, const FVector& InScale, const UWorld* InWorld )
{
	if(InNiagaraSystem.IsNull())
	{
		LOG_WARN(R4Effect, TEXT("NiagaraSystem Obj is Null."));
		return nullptr;
	}

	const UWorld* world = (InWorld == nullptr) ? R4GetWorld() : InWorld;
	UNiagaraSystem* niagara = InNiagaraSystem.LoadSynchronous();
	
	return UNiagaraFunctionLibrary::SpawnSystemAtLocation(world, niagara, InLocation, InRotation, InScale);
}

/**
* Niagara System을 생성하여 Attach 한다. ( 로컬에서만, 멀티 X )
*/
UNiagaraComponent* UtilEffect::SpawnNiagaraAttached_Local(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem, USceneComponent* InAttachComp, FName InSocketName, const FVector& InLocation, const FRotator& InRotation )
{
	if(InNiagaraSystem.IsNull())
	{
		LOG_WARN(R4Effect, TEXT("NiagaraSystem Obj is Null."));
		return nullptr;
	}

	UNiagaraSystem* niagara = InNiagaraSystem.LoadSynchronous();
	
	return UNiagaraFunctionLibrary::SpawnSystemAttached(niagara, InAttachComp, InSocketName, InLocation, InRotation, EAttachLocation::KeepRelativeOffset, true );
}
