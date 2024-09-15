// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ForSpecificID.h"

#include <Animation/AnimMontage.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_ForSpecificID)

UAnimNotify_ForSpecificID::UAnimNotify_ForSpecificID()
{
	bIsNativeBranchingPoint = true;
	
	NotifyType = ER4AnimNotifyType::None;
}

/**
 *	Notify Type을 이름으로 return.
 */
FString UAnimNotify_ForSpecificID::GetNotifyName_Implementation() const
{
	return ENUM_TO_STRING( ER4AnimNotifyType, NotifyType );
}

#if WITH_EDITOR
bool UAnimNotify_ForSpecificID::CanBePlaced( UAnimSequenceBase* InAnimation ) const
{
	return (InAnimation && InAnimation->IsA(UAnimMontage::StaticClass()));
}
#endif

void UAnimNotify_ForSpecificID::BranchingPointNotify( FBranchingPointNotifyPayload& InBranchingPointPayload )
{
	Super::BranchingPointNotify( InBranchingPointPayload );

	// 현재 이 Anim을 Play 시킨 MontageInstance ID에만 execute
	if ( auto it = CachedRequest.Find( InBranchingPointPayload.MontageInstanceID ) )
		it->ExecuteIfBound();
}

/**
 *	Notify를 받을 수 있는 Key로 구분된 Delegate 반환, 중복 시 오버라이드 됨.
 *	@param InKey : MontageInstance ID
 */
FSimpleDelegate& UAnimNotify_ForSpecificID::OnNotify( int32 InKey )
{
	return CachedRequest.Emplace( InKey, FSimpleDelegate() );
}

/**
 *	특정 Key에 대해 연결된 Delegate를 Unbind
 *	@param InKey : MontageInstance ID
 */
void UAnimNotify_ForSpecificID::UnbindNotify( int32 InKey )
{
	CachedRequest.Remove( InKey );
}