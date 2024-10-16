// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/Button.h>

#include "R4ImageIdButton.generated.h"

/**
 * Image와 Id를 설정할 수 있고, 클릭 시 ID를 broadcast해주는 Button.
 * FOnImageIdButtonClickedDelegate 를 사용 할 것 !
 */
UCLASS()
class RAID4_API UR4ImageIdButton : public UButton
{
	GENERATED_BODY()

public:
	UR4ImageIdButton();

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface
	
public:
	// 해당 클래스가 Click 시 broadcast할 Id.
	void SetId( int32 InId );

	// Set Image (ASync)
	void SetImage( const TSoftObjectPtr<UTexture>& InImage );
	
	// 설정된 Id를 Return
	FORCEINLINE int32 GetId() const { return CachedId; }
	
	// Click 시 Id를 담아 broadcast.
	DECLARE_MULTICAST_DELEGATE_OneParam( FOnImageIdButtonClickedDelegate, int32 )
	FOnImageIdButtonClickedDelegate OnImageIdButtonClickedDelegate;
	
private:
	// Texture Async Load 성공 시 호출
	UFUNCTION()
	void _TextureLoadComplete();

	// Button Click 시 Id를 담아 broadcast.
	UFUNCTION()
	void _OnImageIdButtonClicked();

	// Image 적용
	void _SetImageByTexture( UTexture* InTexture );
private:
	// Image 크기
	UPROPERTY( EditDefaultsOnly )
	FVector2D ImageSize;
	
	// 설정된 ID
	UPROPERTY( Transient, VisibleInstanceOnly )
	int32 CachedId;
	
	// Texture Async load handle
	TSharedPtr<FStreamableHandle> CachedTextureHandle;
};
