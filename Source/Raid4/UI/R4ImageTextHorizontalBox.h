// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/HorizontalBox.h>
#include "R4ImageTextHorizontalBox.generated.h"

class UImage;
class UTextBlock;

/**
 * Image와 Text를 보여주는 Widget.
 */
UCLASS()
class RAID4_API UR4ImageTextHorizontalBox : public UHorizontalBox
{
	GENERATED_BODY()

public:
	UR4ImageTextHorizontalBox();
	
protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

public:
	// Image 설정 (ASync)
	void SetImage( const TSoftObjectPtr<UTexture>& InImage );
	
	// Text 설정
	void SetText( const FString& InText ) const;

private:
	void _TextureLoadComplete();
	
private:
	// default Image
	UPROPERTY( EditDefaultsOnly )
	TSoftObjectPtr<UTexture> DefaultImage;
	
	// Image 크기
	UPROPERTY( EditDefaultsOnly )
	FVector2D ImageSize;

	// Padding
	UPROPERTY( EditDefaultsOnly )
	FMargin Padding;
	
	// Image
	UPROPERTY( Transient, VisibleInstanceOnly )
	TWeakObjectPtr<UImage> CachedImage;
	
	// Text
	UPROPERTY( Transient, VisibleInstanceOnly )
	TWeakObjectPtr<UTextBlock> CachedText;

	// Texture Async load handle
	TSharedPtr<FStreamableHandle> CachedTextureHandle;
};
