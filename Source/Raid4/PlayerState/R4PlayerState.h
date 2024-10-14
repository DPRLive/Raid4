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
	virtual void CopyProperties(APlayerState* InPlayerState) override;
	
public:
	// ~ Begin IR4PlayerStateInterface
	FORCEINLINE virtual int32 GetCharacterId() const override { return SelectedCharacterId; }
	virtual void SetCharacterId( int32 InCharacterId ) override { SelectedCharacterId = InCharacterId; _OnRep_SelectedCharacterId(); }
	FORCEINLINE virtual FOnSetCharacterIdDelegate& OnSetCharacterId() override { return OnSetCharacterIdDelegate; }
	// ~ End IR4PlayerStateInterface

private:
	UFUNCTION()
	void _OnRep_SelectedCharacterId() const;
	
private:
	// 선택한 Character Number (DT Key)
	UPROPERTY( ReplicatedUsing = _OnRep_SelectedCharacterId, VisibleInstanceOnly, Transient )
	int32 SelectedCharacterId;

	// Character ID 선택 Delegate.
	FOnSetCharacterIdDelegate OnSetCharacterIdDelegate;
};
