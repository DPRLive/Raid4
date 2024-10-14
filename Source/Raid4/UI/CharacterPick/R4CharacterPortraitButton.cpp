// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterPortraitButton.h"
#include "../../Data/Character/R4CharacterRow.h"
#include "../../Data/Character/R4CharacterSrcRow.h"

#include <Engine/AssetManager.h>
#include <Engine/Texture.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CharacterPortraitButton)

UR4CharacterPortraitButton::UR4CharacterPortraitButton()
{
	CachedCharacterId = 0;
	CachedTextureHandle = nullptr;
}

/**
 *	Change button Image
 *	@param InPk : 선택한 Character DT.
 */
void UR4CharacterPortraitButton::SetCharacterDTKey( FPriKey InPk )
{
	const FR4CharacterRowPtr characterData( InPk );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterData is Invalid. PK : [%d]"), InPk );
		return;
	}

	// Get Resource Pk
	const FR4CharacterSrcRowPtr characterSrcRow( characterData->ResourceRowPK );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterSrcData is Invalid. PK : [%d]"), InPk );
		return;
	}

	// Set key
	CachedCharacterId = InPk;
	OnClicked.AddUniqueDynamic( this, &UR4CharacterPortraitButton::_OnPortraitButtonClicked );
	
	// Async Load Portrait Texture
	CachedTextureHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad
	(
		characterSrcRow->Portrait.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject( this, &UR4CharacterPortraitButton::_TextureLoadComplete )
	);
}

/**
 *	Button Img 설정
 */
void UR4CharacterPortraitButton::_TextureLoadComplete()
{
	if ( !CachedTextureHandle.IsValid() )
	{
		LOG_WARN( R4Data, TEXT("Texture Data Load Failed.") );
		return;
	}

	// Normal, Hover, Pressed, Disabled 모두 설정.
	if ( UTexture* portrait = Cast<UTexture>( CachedTextureHandle->GetLoadedAsset() ) )
	{
		FButtonStyle style = GetStyle();

		style.Normal.SetResourceObject( portrait );
		style.Hovered.SetResourceObject( portrait );
		style.Pressed.SetResourceObject( portrait );
		style.Disabled.SetResourceObject( portrait );
		
		SetStyle( style );
	}

	CachedTextureHandle->ReleaseHandle();
}

/**
 *	DT Key를 담아 broadcast
 */
void UR4CharacterPortraitButton::_OnPortraitButtonClicked()
{
	if ( OnPortraitButtonClickedDelegate.IsBound() )
		OnPortraitButtonClickedDelegate.Broadcast( CachedCharacterId );
}
