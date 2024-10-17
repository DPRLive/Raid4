// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include <Blueprint/IUserObjectListEntry.h>

#include "R4SessionEntryWidget.generated.h"

struct FR4SessionEntryInfo
{
	FR4SessionEntryInfo ()
	: ResultIndex( INDEX_NONE )
	, Name( FString() )
	, NowPlayer( 0 )
	, MaxPlayer( 0 )
	, Ping( 0 )
	{}

	// FOnlineSessionSearchResult 배열의 Index
	int32 ResultIndex;

	// Host Name
	FString Name;

	// 현재 Player
	int32 NowPlayer;

	// 최대 플레이어
	int32 MaxPlayer;

	// 핑
	int32 Ping;
};

class UTextBlock;

/**
 * Session List에 표시되는 Session List Entry.
 */
UCLASS()
class RAID4_API UR4SessionEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UR4SessionEntryWidget( const FObjectInitializer& InObjectInitializer );

protected:
	// ~ Begin IUserObjectListEntry
	virtual void NativeOnListItemObjectSet( UObject* InListItemObject ) override;
	// ~ End IUserObjectListEntry

public:
	// Session Entry Info 설정
	void SetSessionEntryInfo( const FR4SessionEntryInfo& InEntryInfo );

	// Session Entry info Get
	FORCEINLINE const FR4SessionEntryInfo& GetSessionEntryInfo() const { return CachedSessionInfo; }
	
private:
	// Name
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UTextBlock> NameText;
	
	// NumPlayer
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UTextBlock> NumPlayerText;
	
	// Ping
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UTextBlock> PingText;
	
	// 현재 표시하고 있는 Session의 Info.
	FR4SessionEntryInfo CachedSessionInfo;
};
