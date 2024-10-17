// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>

#include "R4PlayerPickWidget.generated.h"

class UTextBlock;
class UImage;

/**
 * Character Picker에서 Player가 Pick한 Character의 Portrait와 Player Name을 보여주는 Widget.
 */
UCLASS()
class RAID4_API UR4PlayerPickWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UR4PlayerPickWidget( const FObjectInitializer& InObjectInitializer );

public:
	// Portrait 설정
	void SetPortrait( const TSoftObjectPtr<UTexture>& InTexture );
	
	// PlayerName 설정
	void SetPlayerName( const FString& InName );

private:
	void _TextureLoadComplete();
	
private:
	// Character Portrait Image
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UImage> CharacterPortraitImage;
	
	// Name
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UTextBlock> PlayerNameText;
	
	// Texture Async load handle
	TSharedPtr<FStreamableHandle> CachedTextureHandle;
};
