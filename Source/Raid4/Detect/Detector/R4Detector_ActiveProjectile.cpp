// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Detector_ActiveProjectile.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Detector_ActiveProjectile)

AR4Detector_ActiveProjectile::AR4Detector_ActiveProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	Direction = FVector( 1.f, 0.f, 0.f );
	Speed = 0.f;
}

void AR4Detector_ActiveProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	FVector deltaLoc = Direction * Speed * DeltaTime;
	AddActorLocalOffset( deltaLoc, true );
}
