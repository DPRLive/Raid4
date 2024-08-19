// Fill out your copyright notice in the Description page of Project Settings.


#include "R4StackProgressBar.h"

#include <Materials/MaterialInstance.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Kismet/KismetMaterialLibrary.h>
#include <Components/Image.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4StackProgressBar)

void UR4StackProgressBar::NativePreConstruct()
{
	Super::NativePreConstruct();

	MaterialInstanceDynamic = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), MaterialInstance);
	ProgressBarImage->SetBrushFromMaterial(MaterialInstanceDynamic);
}

/**
 *  Bottom Progress의 비율을 설정. (오른쪽)
 *  @param InRatio : 설정할 비율. 0.f ~ 1.f
 */
void UR4StackProgressBar::SetBottomProgressRatio(float InRatio)
{
	MaterialInstanceDynamic->SetScalarParameterValue(TEXT("BottomProgress"), InRatio);
}

/**
 *  Top Progress의 비율을 설정 (왼쪽)
 *  @param InRatio : 설정할 비율. 0.f ~ 1.f
 */
void UR4StackProgressBar::SetTopProgressRatio(float InRatio)
{
	MaterialInstanceDynamic->SetScalarParameterValue(TEXT("TopProgress"), InRatio);
}
