// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Skill_PlayerCombo.h"

#include "../../Animation/Notify/R4NotifyByIdInterface.h"

#include <Net/UnrealNetwork.h>
#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>

// TEST
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Skill_PlayerCombo)

UR4Skill_PlayerCombo::UR4Skill_PlayerCombo()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault( true );
}

#if WITH_EDITOR
void UR4Skill_PlayerCombo::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
	Super::PostEditChangeProperty( PropertyChangedEvent );

	if(PropertyChangedEvent.MemberProperty == nullptr)
		return;
	
	// 변경된 프로퍼티가 FSkillAnimInfo 형식이면, 해당 Anim에서 Notify를 읽어와 배열을 자동으로 채움. 하하 아주 편리하지?
	if ( FStructProperty* prop = CastField<FStructProperty>( PropertyChangedEvent.MemberProperty );
		prop != nullptr &&
		prop->Struct == FR4SkillAnimInfo::StaticStruct() )
	{
		FR4SkillAnimInfo* animInfo = prop->ContainerPtrToValuePtr<FR4SkillAnimInfo>( this );
		UAnimMontage* anim = animInfo->SkillAnim;

		if ( !IsValid( anim ) )
			return;
		
		// ComboInputTest Notify의 index들을 찾아냄 
		TSet<int32> idxs;
		for ( int32 i = 0; i < anim->Notifies.Num(); i++ )
		{
			IR4NotifyByIdInterface* detectNotify = Cast<IR4NotifyByIdInterface>( anim->Notifies[i].Notify );
			if ( detectNotify == nullptr || detectNotify->GetNotifyType() != ER4AnimNotifyType::ComboInputTest )
				return;
			
			idxs.Emplace( i );
		}
		
		// 필요 없는 Anim Notifies index는 제거
		for ( auto it = ComboInputInfo.CreateIterator(); it; ++it )
		{
			if ( idxs.Find( it->NotifyNumber ) == nullptr )
			{
				it.RemoveCurrentSwap();
				continue;
			}
			idxs.Remove( it->NotifyNumber );
		}
		
		// 기존에 없는 index는 추가
		for ( const auto& idx : idxs )
		{
			if ( ComboInputInfo.FindByPredicate( [idx](const FR4ComboInputInfo& InElem)
				{ return InElem.NotifyNumber == idx; } ) == nullptr )
				ComboInputInfo.Emplace( idx, INDEX_NONE, NAME_None, NAME_None );
		}
	
		// sort by notify number
		ComboInputInfo.Sort( [](const FR4ComboInputInfo& InElem1, const FR4ComboInputInfo& InElem2 )
		{
			return InElem1.NotifyNumber < InElem2.NotifyNumber;
		} );

		// 해당 Notifies가 속하는 Now Section의 정보를 파싱해서 채움
		for(auto& comboInfo : ComboInputInfo)
		{
			float triggerTime = anim->Notifies[comboInfo.NotifyNumber].GetTriggerTime();
			comboInfo.NowSectionIndex = anim->GetSectionIndexFromPosition( triggerTime );
			comboInfo.NowSectionName = anim->GetSectionName( comboInfo.NowSectionIndex );
		}
	}
}
#endif

void UR4Skill_PlayerCombo::GetLifetimeReplicatedProps( TArray<class FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	DOREPLIFETIME_CONDITION( UR4Skill_PlayerCombo, Server_CachedOnComboInput, COND_OwnerOnly );
}

/**
 *  스킬 입력 시작
 */
void UR4Skill_PlayerCombo::OnInputStarted()
{
	// Combo Skill 사용 시작인 경우
	if ( !CachedIsComboSkillActive )
	{
		// 스킬 사용 가능 상태인지 확인
		if ( !CanActivateSkill() )
			return;

		// Anim Play.
		PlaySkillAnim( ComboSkillAnimInfo );
	}
	// Combo Skill 사용 중인 경우
	else
	{
		// Combo Input이 입력 된 상태가 아니면 요청
		if ( !Server_CachedOnComboInput )
			_ServerRPC_RequestComboInput();
	}
}

/**
 * Anim을 Play시작 시 호출. Server와 Owner Client 에서 호출.
 * @param InInstanceID : 부여된 MontageInstanceID
 * @param InSkillAnimInfo : Play될 Skill Anim 정보
 */
void UR4Skill_PlayerCombo::OnBeginSkillAnim( int32 InInstanceID, const FR4SkillAnimInfo& InSkillAnimInfo )
{
	if ( InSkillAnimInfo.SkillAnimServerKey == ComboSkillAnimInfo.SkillAnimServerKey )
	{
		// InputTest Notify <-> InputTest Bind
		_BindNotifiesAndInputTest( InInstanceID );
		CachedIsComboSkillActive = true;
	}
}

/**
 *  Anim 종료 시 호출. Server와 Owner Client 에서 호출
 * @param InInstanceID : Play시 부여된 MontageInstanceID
 * @param InSkillAnimInfo : End될 Skill Anim 정보
 */
void UR4Skill_PlayerCombo::OnEndSkillAnim( int32 InInstanceID, const FR4SkillAnimInfo& InSkillAnimInfo, bool InIsInterrupted )
{
	if ( InSkillAnimInfo.SkillAnimServerKey == ComboSkillAnimInfo.SkillAnimServerKey )
	{
		// InputTest Notify <-> InputTest Unbind
		_UnbindNotifiesAndInputTest( InInstanceID );
		CachedIsComboSkillActive = false;
	
		if ( GetOwnerRole() == ROLE_Authority )
			Server_CachedOnComboInput = false;

		// Combo Skill의 경우, 종료 시점에 SetCoolTime
		SetSkillCoolDownTime( GetSkillCoolDownTime( false ) );
	}
}

/**
 * Skill Anim 을 현재 Play할 수 없는지 확인.
 * Client에서 PlaySkillAnim시에 확인 및 PlayAnim Server RPC에서 Validation Check에 사용
 * @param InSkillAnimInfo : Play할 Skill Anim
 */
bool UR4Skill_PlayerCombo::IsLockPlaySkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo ) const
{
	if(InSkillAnimInfo.SkillAnimServerKey == ComboSkillAnimInfo.SkillAnimServerKey)
	{
		// Combo Skill이 진행중이거나 Skill을 사용할 수 없을 때 Lock
		return CachedIsComboSkillActive || !CanActivateSkill();
	}
	
	return true;
}

/**
 * Server로 ComboInput을 요청
 */
void UR4Skill_PlayerCombo::_ServerRPC_RequestComboInput_Implementation()
{
	Server_CachedOnComboInput = true;
}

/**
 * Server로 ComboInput을 요청 유효성 확인
 */
bool UR4Skill_PlayerCombo::_ServerRPC_RequestComboInput_Validate()
{
	// Combo Skill을 사용중일때, 유효함으로 판정
	return CachedIsComboSkillActive;
}

/**
 * Combo Input Test
 * Input Test 시점 이전에 입력이 들어와 있으면 (Server_CachedOnComboInput) 다음 Anim으로 Transition
 * @param InNotifyNumber : Input Test를 진행하라고 알린 Notify의 Index
 */
void UR4Skill_PlayerCombo::_ComboInputTest( uint8 InNotifyNumber )
{
	if ( Server_CachedOnComboInput )
	{
		// Combo Input Notify 정보를 Notify 기준으로 Sort해 놓았으므로 binary_search
		int32 index = Algo::LowerBound( ComboInputInfo, InNotifyNumber,
			[](const FR4ComboInputInfo& InElem1, int32 InIndex)
			{
				return InElem1.NotifyNumber < InIndex; 
			});
		
		// 다음 Section이 잘 존재할 때
		if ( ( ComboInputInfo.IsValidIndex( index ) ) && ( ComboInputInfo[index].NotifyNumber == InNotifyNumber) )
		{
			// TODO : Find Section Name & transition

			Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance()->Montage_JumpToSection( ComboInputInfo[index].NextSectionName );
			
			// Section Transition In Local
			if ( GetOwnerRole() == ROLE_AutonomousProxy )
			{
				
			}
			// Section Transition Server
			else if ( GetOwnerRole() == ROLE_Authority )
			{
				// false로 변경
				Server_CachedOnComboInput = false;
			}
		}
	}
}

/**
 *  InputTest Notify <-> InputTest Bind
 *  @param InMontageInstanceId : Notify delegate bind 시 구별할 MontageInstance ID
 */
void UR4Skill_PlayerCombo::_BindNotifiesAndInputTest( int32 InMontageInstanceId )
{
	if( !ensureMsgf( IsValid(ComboSkillAnimInfo.SkillAnim) , TEXT("Skill Anim is Invalid.")))
		return;
	
	for ( const auto& comboInfo : ComboInputInfo )
	{
		if( !ensureMsgf( ComboSkillAnimInfo.SkillAnim->Notifies.IsValidIndex( comboInfo.NotifyNumber ),
			TEXT("Notify index is Invalid.")))
			return;

		if( IR4NotifyByIdInterface* comboInputTestNotifyObj = Cast<IR4NotifyByIdInterface>(ComboSkillAnimInfo.SkillAnim->Notifies[comboInfo.NotifyNumber].Notify ) )
		{
			// this 캡처, WeakLambda 사용
			comboInputTestNotifyObj->OnNotify( InMontageInstanceId ).BindWeakLambda(this,
				[this, notifyNum = comboInfo.NotifyNumber]()
				{
					// Input Test 실행
					_ComboInputTest( notifyNum );
				});
		}
	}
}

/**
 *  InputTest Notify <-> InputTest Unbind
 *  @param InMontageInstanceId : Notify delegate bind 시 구별할 MontageInstance ID
 */
void UR4Skill_PlayerCombo::_UnbindNotifiesAndInputTest( int32 InMontageInstanceId )
{
	if( !ensureMsgf( IsValid(ComboSkillAnimInfo.SkillAnim) , TEXT("Skill Anim is Invalid.")))
		return;
	
	for ( const auto& comboInfo : ComboInputInfo )
	{
		if( !ensureMsgf( ComboSkillAnimInfo.SkillAnim->Notifies.IsValidIndex( comboInfo.NotifyNumber ),
			TEXT("Notify index is Invalid.")))
			return;

		if( IR4NotifyByIdInterface* comboInputTestNotifyObj = Cast<IR4NotifyByIdInterface>(ComboSkillAnimInfo.SkillAnim->Notifies[comboInfo.NotifyNumber].Notify ) )
			comboInputTestNotifyObj->UnbindNotify( InMontageInstanceId );
	}
}
