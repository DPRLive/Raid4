// Fill out your copyright notice in the Description page of Project Settings.


#include "R4ImageIdButton.h"

#include <Engine/AssetManager.h>
#include <Engine/Texture.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4ImageIdButton)

UR4ImageIdButton::UR4ImageIdButton()
{
	CachedId = 0;
	ImageSize = FVector2D( 50.f );
	CachedTextureHandle = nullptr;
}

TSharedRef<SWidget> UR4ImageIdButton::RebuildWidget()
{
	TSharedRef<SWidget> result = Super::RebuildWidget();
	
	OnClicked.AddUniqueDynamic( this, &UR4ImageIdButton::_OnImageIdButtonClicked );
	
	return result;
}

/**
 *	해당 클래스가 Click 시 broadcast할 Id.
 *	@param InId : Click 시 Broadcast할 Id.
 */
void UR4ImageIdButton::SetId( int32 InId )
{
	// Set key
	CachedId = InId;
}

/**
 *	Set Image (ASync)
 */
void UR4ImageIdButton::SetImage( const TSoftObjectPtr<UTexture>& InImage )
{
	if ( InImage.IsNull() )
	{
		_SetImageByTexture( nullptr );
		return;
	}
	
	// Async Load Image Texture
	CachedTextureHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad
	(
		InImage.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject( this, &UR4ImageIdButton::_TextureLoadComplete )
	);
}

/**
 *	Button Img 설정
 */
void UR4ImageIdButton::_TextureLoadComplete()
{
	if ( !CachedTextureHandle.IsValid() )
	{
		LOG_WARN( R4Data, TEXT("Texture Data Load Failed.") );
		return;
	}

	if ( UTexture* img = Cast<UTexture>( CachedTextureHandle->GetLoadedAsset() ) )
		_SetImageByTexture( img );

	CachedTextureHandle->ReleaseHandle();
}

/**
 * Id를 담아 broadcast
 */
void UR4ImageIdButton::_OnImageIdButtonClicked()
{
	if ( OnImageIdButtonClickedDelegate.IsBound() )
		OnImageIdButtonClickedDelegate.Broadcast( CachedId );
}

/**
 *	Image 적용
 */
void UR4ImageIdButton::_SetImageByTexture( UTexture* InTexture )
{
	FButtonStyle style = GetStyle();

	// Normal, Hover, Pressed, Disabled 모두 설정.
	style.Normal.SetResourceObject( InTexture );
	style.Hovered.SetResourceObject( InTexture );
	style.Pressed.SetResourceObject( InTexture );
	style.Disabled.SetResourceObject( InTexture );
	
	// Set Size
	style.Normal.SetImageSize( ImageSize );
	style.Hovered.SetImageSize( ImageSize );
	style.Pressed.SetImageSize( ImageSize );
	style.Disabled.SetImageSize( ImageSize );
	
	SetStyle( style );

	RebuildWidget();
}
