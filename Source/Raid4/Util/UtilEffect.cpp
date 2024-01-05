#include "UtilEffect.h"

#include <NiagaraSystem.h>
#include <NiagaraFunctionLibrary.h>
#include <NiagaraComponent.h>

/**
* Niagara System을 생성한다. ( 로컬에서만, 멀티 X )
*/
UNiagaraComponent* UtilEffect::SpawnNiagaraAtLocation_Local(const TSoftObjectPtr<UNiagaraSystem>& InNiagaraSystem,
                                                             const FVector& InLocation, const FRotator& InRotation, const FVector& InScale, UWorld* InWorld)
{
	if(InNiagaraSystem.IsNull())
	{
		LOG_WARN(R4Effect, TEXT("NiagaraSystem Obj is Null."));
		return nullptr;
	}

	UNiagaraSystem* niagara = nullptr;
	
	if(InNiagaraSystem.IsPending())
		niagara = InNiagaraSystem.LoadSynchronous();
	else
		niagara = InNiagaraSystem.Get();
	
	return UNiagaraFunctionLibrary::SpawnSystemAtLocation(InWorld, niagara, InLocation, InRotation, InScale);
}
