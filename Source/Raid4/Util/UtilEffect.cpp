#include "UtilEffect.h"

#include <NiagaraSystem.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>

/**
* Niagara System을 생성한다. ( 로컬에서만, 멀티 X )
*/
UNiagaraComponent* UtilEffect::SpawnNiagaraAtLocation_Local(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem,
                                                             const FVector& InLocation, const FRotator& InRotation, const FVector& InScale, UWorld* InWorld )
{
	if(InNiagaraSystem.IsNull())
	{
		LOG_WARN(R4Effect, TEXT("NiagaraSystem Obj is Null."));
		return nullptr;
	}

	UWorld* world = (InWorld == nullptr) ? R4GetWorld() : InWorld;
	UNiagaraSystem* niagara = LoadSoftObjectSync<UNiagaraSystem>(InNiagaraSystem);
	
	return UNiagaraFunctionLibrary::SpawnSystemAtLocation(world, niagara, InLocation, InRotation, InScale);
}
