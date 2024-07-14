// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "R4UserWidget.generated.h"

/**
 * Owing Actor를 저장하기 위한 UserWidget의 부모 클래스.
 */
UCLASS()
class RAID4_API UR4UserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetOwningActor(AActor* NewOwner) { OwningActor = NewOwner; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actor")
	TWeakObjectPtr<AActor> OwningActor;
};
