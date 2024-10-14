// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PreviewCharacterPawn.h"
#include "../Character/R4CharacterRow.h"
#include "../PlayerState/R4PlayerStateInterface.h"

#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Components/SkeletalMeshComponent.h>
#include <Engine/SkeletalMesh.h>
#include <Engine/AssetManager.h>
#include <GameFramework/PlayerState.h>

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
	SkeletalMeshComp->SetupAttachment( RootComponent );
}

void AR4PreviewCharacterPawn::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  Player State로부터 Character DT Key 값 Load (Server)
 */
void AR4PreviewCharacterPawn::PossessedBy( AController* InNewController )
{
	Super::PossessedBy( InNewController );

	if( !IsLocallyControlled() )
		return;
	
	if ( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( GetPlayerState() ) )
	{
		PushDTData( playerState->GetCharacterId() );

		// 변경사항 수신
		playerState->OnSetCharacterId().AddUObject( this, &AR4PreviewCharacterPawn::PushDTData );
	}
}

/**
 *  Player State로부터 Character DT Key 값 Load (Client)
 */
void AR4PreviewCharacterPawn::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if( !IsLocallyControlled() )
		return;
	
	if ( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( GetPlayerState() ) )
	{
		PushDTData( playerState->GetCharacterId() );

		// 변경사항 수신
		playerState->OnSetCharacterId().AddUObject( this, &AR4PreviewCharacterPawn::PushDTData );
	}
}

/**
 *	Mesh Change & Play Level Start Anim
 *	@param InPk : 선택한 Character DT.
 */
void AR4PreviewCharacterPawn::PushDTData( FPriKey InPk )
{
	if( InPk == DTConst::G_InvalidPK )
		return;
	
	const FR4CharacterRowPtr characterData( InPk );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterData is Invalid. PK : [%d]"), InPk );
		return;
	}

	ClearDTData();
	
	CachedAnimInstance = characterData->AnimInstance;
	CachedPickedAnimMontage = characterData->CharacterPickedAnim.LoadSynchronous();
	
	SkeletalMeshComp->SetRelativeTransform( characterData->MeshTransform );
	
	// Skeletal Mesh ( Async load )
	CachedMeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad
	(
		characterData->SkeletalMesh.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject( this, &AR4PreviewCharacterPawn::_MeshLoadComplete )
	);
}

void AR4PreviewCharacterPawn::ClearDTData()
{
	SkeletalMeshComp->SetSkeletalMesh( nullptr );
	CachedAnimInstance = nullptr;
	CachedPickedAnimMontage = nullptr;
}

/**
 *	Mesh Load Completed
 */
void AR4PreviewCharacterPawn::_MeshLoadComplete()
{
	if ( !CachedMeshHandle.IsValid() )
	{
		LOG_WARN( R4Data, TEXT("Mesh Data Load Failed.") );
		return;
	}
	
	if ( USkeletalMesh* skelMesh = Cast<USkeletalMesh>( CachedMeshHandle->GetLoadedAsset() ) )
		SkeletalMeshComp->SetSkeletalMesh( skelMesh );

	CachedMeshHandle->ReleaseHandle();
	
	// Anim
	SkeletalMeshComp->SetAnimInstanceClass( CachedAnimInstance );
	if( UAnimInstance* animInstance = SkeletalMeshComp->GetAnimInstance() )
		animInstance->Montage_Play( CachedPickedAnimMontage );
}
