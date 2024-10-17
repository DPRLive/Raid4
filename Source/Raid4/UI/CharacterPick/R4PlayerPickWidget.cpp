// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerPickWidget.h"

#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Engine/AssetManager.h>
#include <Engine/Texture.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerPickWidget)

UR4PlayerPickWidget::UR4PlayerPickWidget( const FObjectInitializer& InObjectInitializer )
	: Super( InObjectInitializer )
{
	CachedTextureHandle = nullptr;
}

/**
 *	Player Portrait 설정
 */
void UR4PlayerPickWidget::SetPortrait( const TSoftObjectPtr<UTexture>& InTexture )
{
	// Image ASync Load
	if ( IsValid( CharacterPortraitImage ) )
	{
		// Async Load Image Texture
		CachedTextureHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad
		(
			InTexture.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject( this, &UR4PlayerPickWidget::_TextureLoadComplete )
		);
	}
}

/**
 *	Player Name 설정
 */
void UR4PlayerPickWidget::SetPlayerName( const FString& InName )
{
	// Player Name
	if ( IsValid( PlayerNameText ) )
		PlayerNameText->SetText( FText::FromString( InName ) );
}

/**
 *	ASync Load Complete, Set Portrait
 */
void UR4PlayerPickWidget::_TextureLoadComplete()
{
	if ( !CachedTextureHandle.IsValid() )
	{
		LOG_WARN( R4Data, TEXT("Texture Data Load Failed.") );
		return;
	}

	if ( UTexture* portrait = Cast<UTexture>( CachedTextureHandle->GetLoadedAsset() ) )
		CharacterPortraitImage->SetBrushResourceObject( portrait );
	
	CachedTextureHandle->ReleaseHandle();
}
