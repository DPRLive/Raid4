// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Blueprint/UserWidget.h>
#include "R4StackProgressBar.generated.h"

class UImage;
class UMaterialInstance;

/**
 * 2가지 색을 표현할 수 있는 Stack Progress Bar.
 * M_StackedProgressBar 기반 Material Instance를 등록하여 사용.
 */
UCLASS()
class RAID4_API UR4StackProgressBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;

public:
	// Bottom Progress의 비율을 설정. (오른쪽)
	UFUNCTION( BlueprintCallable )
	void SetBottomProgressRatio(float InRatio);

	// Top Progress의 비율을 설정 (왼쪽)
	UFUNCTION( BlueprintCallable )
	void SetTopProgressRatio(float InRatio);
	
private:
	// Material Instance
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true) )
	TObjectPtr<UMaterialInstance> MaterialInstance;

	// Dynamic Material Instance
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, Transient, meta = (AllowPrivateAccess = true) )
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstanceDynamic;

	// ProgressBar Image
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite, meta = (bindWidget, AllowPrivateAccess = true) )
	TObjectPtr<UImage> ProgressBarImage;
};
