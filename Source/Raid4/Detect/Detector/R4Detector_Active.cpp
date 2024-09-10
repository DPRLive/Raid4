// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Detector_Active.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Detector_Active)

AR4Detector_Active::AR4Detector_Active()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AR4Detector_Active::BeginPlay()
{
	Super::BeginPlay();
	
}

void AR4Detector_Active::PreReturnPoolObject()
{
	Super::PreReturnPoolObject();
	
	OnBeginDetectDelegate.Clear();
	OnEndDetectDelegate.Clear();
}

/**
 *	Detect 실행
*	@param InOrigin : 탐지의 기준이 되는 Transform
 *	@param InDetectDesc : Detect 실행에 필요한 Param
 */
void AR4Detector_Active::ExecuteDetect( const FTransform& InOrigin, const FR4DetectDesc& InDetectDesc )
{
	SetActorLocation( InOrigin.GetLocation() );
}
