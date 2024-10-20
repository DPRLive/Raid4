﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PreviewCharacterPawn.h"
#include "../Data/Character/R4CharacterRow.h"
#include "../Data/Character/R4CharacterSrcRow.h"

#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Components/SkeletalMeshComponent.h>
#include <Engine/SkeletalMesh.h>
#include <Engine/AssetManager.h>
#include <Engine/SkinnedAssetCommon.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PreviewCharacterPawn)

AR4PreviewCharacterPawn::AR4PreviewCharacterPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	// Not replicate
	bReplicates = false;
	bOnlyRelevantToOwner = true;
	bNetLoadOnClient = false;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent( RootComponent );
	
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(  TEXT("SkeletalMeshComp") );
	if ( SkeletalMeshComp )
		SkeletalMeshComp->SetupAttachment( RootComponent );
}

void AR4PreviewCharacterPawn::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *	Mesh Change & Play Level Start Anim
 *	@param InPk : 선택한 Character DT.
 */
void AR4PreviewCharacterPawn::PushDTData( FPriKey InPk )
{
	if( InPk == DTConst::G_InvalidPK )
		return;
	
	const FR4CharacterRowPtr characterData( GetWorld(), InPk );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterData is Invalid. PK : [%d]"), InPk );
		return;
	}

	// Get Resource Pk
	const FR4CharacterSrcRowPtr characterSrcRow( GetWorld(), characterData->ResourceRowPK );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterSrcData is Invalid. PK : [%d]"), InPk );
		return;
	}
	
	ClearDTData();
	
	CachedAnimInstance = characterData->AnimInstance;
	CachedPickedAnimMontage = characterSrcRow->CharacterPickedAnim.LoadSynchronous();

	if ( IsValid( SkeletalMeshComp ) )
		SkeletalMeshComp->SetRelativeTransform( characterSrcRow->MeshTransform );
	
	// Skeletal Mesh ( Async load )
	CachedMeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad
	(
		characterSrcRow->SkeletalMesh.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject( this, &AR4PreviewCharacterPawn::_MeshLoadComplete )
	);
}

void AR4PreviewCharacterPawn::ClearDTData()
{
	if ( IsValid( SkeletalMeshComp ) )
		SkeletalMeshComp->SetSkeletalMesh( nullptr );
	CachedAnimInstance = nullptr;
	CachedPickedAnimMontage = nullptr;
}

/**
 *	Mesh Load Completed
 */
void AR4PreviewCharacterPawn::_MeshLoadComplete()
{
	if ( !IsValid( SkeletalMeshComp ) )
		return;
	
	if ( !CachedMeshHandle.IsValid() )
	{
		LOG_WARN( R4Data, TEXT("Mesh Data Load Failed.") );
		return;
	}
	
	if ( USkeletalMesh* skelMesh = Cast<USkeletalMesh>( CachedMeshHandle->GetLoadedAsset() ) )
	{
		SkeletalMeshComp->SetSkeletalMesh( skelMesh );

		// Mesh가 존재하는 상태에서 변경 시 Material이 안되는 버그?가 있어서 강제로 설정.
		TArray<FSkeletalMaterial>& matArray = skelMesh->GetMaterials();
		for ( int32 matIndex = 0; matIndex < matArray.Num(); matIndex++ )
			SkeletalMeshComp->SetMaterial( matIndex, matArray[matIndex].MaterialInterface );
	}

	CachedMeshHandle->ReleaseHandle();
	
	// Anim
	SkeletalMeshComp->SetAnimInstanceClass( CachedAnimInstance );
	if( UAnimInstance* animInstance = SkeletalMeshComp->GetAnimInstance() )
		animInstance->Montage_Play( CachedPickedAnimMontage );
}
