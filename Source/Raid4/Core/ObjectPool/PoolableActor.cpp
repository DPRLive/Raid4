// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolableActor.h"

#include <GameFramework/Pawn.h>
#include <GameFramework/GameNetworkManager.h>
#include <Components/SkeletalMeshComponent.h>
#include <Net/UnrealNetwork.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(PoolableActor)

APoolableActor::APoolableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	bControlCollisionByPool = true;
	bActivate = true;
}

/**
 *  Server에서 bHidden && Disable Collision 설정 시 Client side에서 Destroy 당하지 않도록
 *  bHidden && Disable Collision 일 경우에 Relevant를 가지지 않도록 하는 if문을 제거
 */
bool APoolableActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	if (bAlwaysRelevant || IsOwnedBy(ViewTarget) || IsOwnedBy(RealViewer) || this == ViewTarget || ViewTarget == GetInstigator())
	{
		return true;
	}
	if (bNetUseOwnerRelevancy && Owner)
	{
		return Owner->IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	}
	if (bOnlyRelevantToOwner)
	{
		return false;
	}
	if (RootComponent && RootComponent->GetAttachParent() && RootComponent->GetAttachParent()->GetOwner() && (Cast<USkeletalMeshComponent>(RootComponent->GetAttachParent()) || (RootComponent->GetAttachParent()->GetOwner() == Owner)))
	{
		return RootComponent->GetAttachParent()->GetOwner()->IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	}

	if (!RootComponent)
	{
		UE_LOG(LogNet, Warning, TEXT("Actor %s / %s has no root component in AActor::IsNetRelevantFor. (Make bAlwaysRelevant=true?)"), *GetClass()->GetName(), *GetName() );
		return false;
	}

	return !GetDefault<AGameNetworkManager>()->bUseDistanceBasedRelevancy ||
			IsWithinNetRelevancyDistance(SrcLocation);
}

void APoolableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APoolableActor, bActivate);
}

/**
 *  Actor를 Pool에 반납하기 위해 비활성화
 */
void APoolableActor::DeactivateActor()
{
	if(HasAuthority())
	{
		// Replicate 휴면 액터로 변경
		SetNetDormancy(DORM_DormantAll);
		FlushNetDormancy();
		
		SetActorHiddenInGame(true);
		DisableCollisionAndTick();
		
		bActivate = false;
	}
}

/**
 *  Actor를 Pool에서 꺼내기 위해 활성화
 */
void APoolableActor::ActivateActor()
{
	if(HasAuthority())
	{
		// Replicate 휴먼 해제
		SetNetDormancy(DORM_Awake);
		SetActorHiddenInGame(false);
		ResetCollisionAndTick();

		bActivate = true;
	}
}

/**
 *  Collision과 Tick을 자식 컴포넌트까지 모두 비활성화
 *  Actor의 bHidden 속성 말고는 Replicate 되지 않아 직접 Replicate
 */
void APoolableActor::DisableCollisionAndTick()
{
	if ( bControlCollisionByPool )
		SetActorEnableCollision(false); // Component 까지 알아서 꺼버림

	SetActorTickEnabled(false);

	TArray<UActorComponent*> comps;
	GetComponents(comps);
	
	for(UActorComponent* comp : comps)
		comp->SetComponentTickEnabled(false);
}

/**
 *  Collision과 Tick을 자식 컴포넌트까지 원래 CDO (UCLASS) 대로 복구
 */
void APoolableActor::ResetCollisionAndTick()
{
	UClass* uClass = GetClass();
	if(!IsValid(uClass))
		return;
	
	if(const AActor* cdo = uClass->GetDefaultObject<AActor>(); IsValid(cdo))
	{
		if ( bControlCollisionByPool )
			SetActorEnableCollision(cdo->GetActorEnableCollision());
		SetActorTickEnabled(cdo->IsActorTickEnabled());
	}

	TArray<UActorComponent*> comps;
	GetComponents(comps);
	for(UActorComponent* comp : comps)
	{
		UClass* compUClass = comp->GetClass();
		if(!IsValid(compUClass))
			continue;
		
		if(const UActorComponent* cdo = compUClass->GetDefaultObject<UActorComponent>(); IsValid(cdo))
		{
			comp->SetComponentTickEnabled(cdo->IsComponentTickEnabled());
		}
	}
}

/**
 *  OnRep_IsActivate
 */
void APoolableActor::OnRep_Activate()
{
	bActivate ? ResetCollisionAndTick() : DisableCollisionAndTick();
}