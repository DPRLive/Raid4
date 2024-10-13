// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PreviewChampionPawn.h"
#include "../Character/R4CharacterRow.h"

#include <Components/SkeletalMeshComponent.h>
#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Engine/SkeletalMesh.h>

#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PreviewChampionPawn)

AR4PreviewChampionPawn::AR4PreviewChampionPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT("SkeletalMeshComp") );
	SkeletalMeshComp->SetupAttachment( RootComponent );
}

void AR4PreviewChampionPawn::BeginPlay()
{
	Super::BeginPlay();

	// 테스또

	FTimerHandle handle;
	GetWorldTimerManager().SetTimer( handle, [this]()
	{
		PushDTData( 1 );
	}, 2.f, false );
}

/**
 *	Mesh Change & Play Level Start Anim
 *	@param InPk : 선택한 Character DT.
 */
void AR4PreviewChampionPawn::PushDTData( FPriKey InPk )
{
	const FR4CharacterRowPtr characterData( InPk );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterData is Invalid. PK : [%d]"), InPk );
		return;
	}
	
	// Skeletal Mesh : 비동기 로드 ?
	if ( USkeletalMesh* skelMesh = characterData->SkeletalMesh.LoadSynchronous() )
	{
		SkeletalMeshComp->SetSkeletalMesh( skelMesh );
		SkeletalMeshComp->SetRelativeTransform( characterData->MeshTransform );
	}
	
	// Anim
	SkeletalMeshComp->SetAnimInstanceClass( characterData->AnimInstance );
	if( UAnimInstance* animInstance = SkeletalMeshComp->GetAnimInstance() )
		animInstance->Montage_Play( characterData->ChampionSelectedAnim.LoadSynchronous() );
}

void AR4PreviewChampionPawn::ClearDTData()
{
	
}