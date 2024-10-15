// Fill out your copyright notice in the Description page of Project Settings.


#include "R4ImageTextHorizontalBox.h"

#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/HorizontalBoxSlot.h>
#include <Engine/Texture.h>
#include <Engine/AssetManager.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4ImageTextHorizontalBox)

UR4ImageTextHorizontalBox::UR4ImageTextHorizontalBox()
{
	DefaultImage = nullptr;
	ImageSize = FVector2D( 100.f );
	Padding = FMargin();
	SetVisibilityInternal( ESlateVisibility::HitTestInvisible );
}

/**
 *	Set Widget
 */
TSharedRef<SWidget> UR4ImageTextHorizontalBox::RebuildWidget()
{
	TSharedRef<SWidget> result = Super::RebuildWidget();

	// Image 추가
	if ( UImage* image = NewObject<UImage>() )
	{
		AddChild( image );
		CachedImage = image;
	}
	
	// Text Block 추가
	if ( UTextBlock* textBlock = NewObject<UTextBlock>() )
	{
		AddChild( textBlock );
		CachedText = textBlock;
	}

	// align & apply padding
	for( UPanelSlot* slot : GetSlots() )
	{
		if ( UHorizontalBoxSlot* boxSlot = Cast<UHorizontalBoxSlot>( slot ) )
		{
			boxSlot->SetHorizontalAlignment( HAlign_Center );
			boxSlot->SetVerticalAlignment( VAlign_Center );
			boxSlot->SetPadding( Padding );
		}
	}

	// set default Image
	SetImage( DefaultImage );
	
	return result;
}

/**
 *	Set Image (ASync)
 */
void UR4ImageTextHorizontalBox::SetImage( const TSoftObjectPtr<UTexture>& InImage )
{
	if( !CachedImage.IsValid() )
		return;
	
	if ( InImage.IsNull() )
	{
		CachedImage->SetBrushResourceObject( nullptr );
		return;
	}
	
	// Async Load Image Texture
	CachedTextureHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad
	(
		InImage.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject( this, &UR4ImageTextHorizontalBox::_TextureLoadComplete )
	);
}

/**
 *	Set Text
 */
void UR4ImageTextHorizontalBox::SetText( const FString& InText ) const
{
	if( CachedText.IsValid() )
		CachedText->SetText( FText::FromString( InText ) );
}

void UR4ImageTextHorizontalBox::_TextureLoadComplete()
{
	if ( !CachedTextureHandle.IsValid() )
	{
		LOG_WARN( R4Data, TEXT("Texture Data Load Failed.") );
		return;
	}

	if ( UTexture* portrait = Cast<UTexture>( CachedTextureHandle->GetLoadedAsset() ) )
	{
		CachedImage->SetBrushResourceObject( portrait );
		CachedImage->SetDesiredSizeOverride( ImageSize );
	}
	
	CachedTextureHandle->ReleaseHandle();
}
