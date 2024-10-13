// Fill out your copyright notice in the Description page of Project Settings.


#include "R4AnimationComponent.h"
#include "../Util/UtilAnimation.h"

#include <GameFramework/Character.h>
#include <Components/SkeletalMeshComponent.h>
#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4AnimationComponent)

UR4AnimationComponent::UR4AnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

/**
 *  ServerTime을 통한 동기화된 Animation Play를 지원. ( InServerTime 기준으로 보정 )
 *  <Start ServerTime과의 Delay의 처리 방식>
 *  Loop Animation의 경우 : Delay 된 StartPos에서 시작
 *  일반 Animation의 경우 : PlayRate를 보정하여 동일 시점에 끝나도록 보정, ( delay > anim length인 경우 : Skip play )
 *  @param InAnimMontage : Play할 Anim Montage
 *  @param InStartSectionName : Play할 Anim Section의 Name
 *  @param InPlayRate : PlayRate, 현재 음수의 play rate는 처리하지 않음.
 *  @param InStartServerTime : 이 Animation을 Play한 서버 시작 시간 
 */
void UR4AnimationComponent::PlayAnimSync( UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate, float InStartServerTime ) const
{
	ACharacter* owner = Cast<ACharacter>( GetOwner() );
	if ( !IsValid( owner ) )
		return;

	UAnimInstance* anim = IsValid( owner->GetMesh() ) ? owner->GetMesh()->GetAnimInstance() : nullptr;
	if ( !IsValid( anim ) )
		return;

	// nullptr이면 정지, 아니면 플레이
	if ( !IsValid( InAnimMontage ) )
	{
		anim->StopAllMontages( anim->Montage_GetBlendTime( nullptr ) );
		return;
	}

	if( InPlayRate < 0.f )
	{
		LOG_WARN( R4Anim, TEXT("Negative InPlayRate [%f] is not processed."), InPlayRate );
		return;
	}
	
	int32 sectionIndex = InAnimMontage->GetSectionIndex( InStartSectionName );
	sectionIndex = ( sectionIndex == INDEX_NONE ) ? 0 : sectionIndex;
	
	// TODO : Server -> Client 상 pkt lag이 있다면, 보정이 힘들 수 있음. ( ServerWorldTimeSecond도 Replicate 되기 때문 )
	float serverTime = R4GetServerTimeSeconds( GetWorld() );
	float delayTime = FMath::Max( 0.f, serverTime - InStartServerTime );
	
	float animTime = UtilAnimation::GetCompositeAnimLength( InAnimMontage, sectionIndex );
	float playRate = FMath::Max( KINDA_SMALL_NUMBER, InPlayRate );
	float startPos = 0.f;
	bool bLoop = FMath::IsNearlyEqual( animTime, -1.f );
	
	// 실제 play되는 anim 시간을 구하기 위해 PlayRate 적용
	animTime /= playRate;
	
	// Loop Animation의 경우 : Delay 된 StartPos에서 시작
	// 일반 Animation의 경우 : 동일 시점에 끝나도록 PlayRate를 보정
	if ( bLoop )
	{
		startPos = UtilAnimation::GetDelayedStartAnimPos( InAnimMontage, sectionIndex, delayTime );
	}
	else if ( delayTime < animTime )
	{
		// playRate = 전체시간 / 남은시간
		playRate = animTime / ( animTime - delayTime );
		
		startPos = InAnimMontage->GetAnimCompositeSection( sectionIndex ).GetTime();
	}
	else // 루프가 아닌데 delay > anim length인 경우 : Skip play 
		return;
	
	// Anim을 Play (Montage Instance가 새로 생성됨)
	anim->Montage_Play( InAnimMontage, playRate, EMontagePlayReturnType::MontageLength, startPos );
}

/**
 *  설정해둔 Dead Animation을 출력.
 */
void UR4AnimationComponent::PlayDeadAnim()
{
	ACharacter* owner = Cast<ACharacter>( GetOwner() );
	if ( !IsValid( owner ) )
		return;
	
	UAnimInstance* anim = IsValid( owner->GetMesh() ) ? owner->GetMesh()->GetAnimInstance() : nullptr;
	if ( !IsValid( anim ) )
		return;
	
	anim->Montage_Play( DeadAnim, 1.f, EMontagePlayReturnType::MontageLength );
}