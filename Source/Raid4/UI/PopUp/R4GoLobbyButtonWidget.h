// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "R4GoLobbyButtonWidget.generated.h"

class UButton;

/**
 * 로비로 돌아가기 버튼
 */
UCLASS()
class RAID4_API UR4GoLobbyButtonWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	// _OnClickGoLobbyButton
	UFUNCTION()
	void _OnClickGoLobbyButton();
	
private:
	// NumPlayer
	UPROPERTY( VisibleAnywhere, meta = ( BindWidget ) )
	TObjectPtr<UButton> GoLobbyButton;
};
