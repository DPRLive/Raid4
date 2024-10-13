// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "R4PlayerStateInterface.h"

#include <GameFramework/PlayerState.h>

#include "R4PlayerState.generated.h"

/**
 * Player State.
 */
UCLASS()
class RAID4_API AR4PlayerState : public APlayerState, public IR4PlayerStateInterface
{
	GENERATED_BODY()

public:
	AR4PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	virtual void CopyProperties(APlayerState* InPlayerState) override;
	
public:
	// ~ Begin IR4PlayerStateInterface
	FORCEINLINE virtual int32 GetCharacterId() const override { return SelectedCharacterId; }
	virtual void SetCharacterId( int32 InCharacterId ) override { SelectedCharacterId = InCharacterId; }
	// ~ End IR4PlayerStateInterface
	
private:
	// 선택한 Character Number (DT Key)
	UPROPERTY( Replicated, VisibleInstanceOnly, Transient )
	int32 SelectedCharacterId;
};
