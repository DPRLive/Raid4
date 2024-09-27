// Fill out your copyright notice in the Description page of Project Settings.


#include "R4ParticleActor.h"

#include <TimerManager.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4ParticleActor)

AR4ParticleActor::AR4ParticleActor()
{
	PrimaryActorTick.bCanEverTick = false;
	LifeTime = 0.f;
}

void AR4ParticleActor::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	TearDown();
	Super::EndPlay( EndPlayReason );
}

void AR4ParticleActor::PreReturnPoolObject()
{
	Super::PreReturnPoolObject();
	
	TearDown();
}

/**
 *	Particle을 Execute.
 *	@param InOrigin : Spawn의 기준이 되는 Origin.
 */
void AR4ParticleActor::Execute( const FTransform& InOrigin )
{
	// Rotation & Position
	SetActorTransform( InOrigin );
	
	// 생명 주기 설정
	// 아주 작으면 다음 틱에 바로 반납
	if ( LifeTime <= KINDA_SMALL_NUMBER )
	{
		GetWorldTimerManager().SetTimerForNextTick( [thisPtr = TWeakObjectPtr<AR4ParticleActor>(this)]
		{
			// AnimMontage editor에서 실행 시 GameInstance가 nullptr로 인식해서 분기. 
			if(thisPtr.IsValid())
			{
				if ( UR4GameInstance* gameInstance = Cast<UR4GameInstance>( thisPtr->GetWorld()->GetGameInstance() ) )
					gameInstance->GetObjectPool()->ReturnPoolObject( thisPtr.Get() );
				else
					thisPtr->Destroy();
			}
		} );
	}
	else
	{
		// 아닌 경우 생명주기 타이머 설정.
		GetWorldTimerManager().SetTimer( LifeTimerHandle,
			[thisPtr = TWeakObjectPtr<AR4ParticleActor>(this)]
			{
				if ( UR4GameInstance* gameInstance = Cast<UR4GameInstance>( thisPtr->GetWorld()->GetGameInstance() ) )
					gameInstance->GetObjectPool()->ReturnPoolObject( thisPtr.Get() );
				else
					thisPtr->Destroy();
			}, LifeTime, false );
	}
	
	BP_Execute( InOrigin );
}

/**
 *	Particle을 정리.
 */
void AR4ParticleActor::TearDown()
{
	BP_TearDown();

	GetWorldTimerManager().ClearTimer( LifeTimerHandle );
}
