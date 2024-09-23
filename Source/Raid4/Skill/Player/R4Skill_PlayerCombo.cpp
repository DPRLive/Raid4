// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Skill_PlayerCombo.h"

#include "../../Animation/Notify/R4NotifyByIdInterface.h"
#include "../../Animation/R4AnimationInterface.h"

#include <Net/UnrealNetwork.h>
#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Skill_PlayerCombo)

UR4Skill_PlayerCombo::UR4Skill_PlayerCombo()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault( true );

	CachedCanComboInput = false;
	CachedOnComboInput = false;
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
				continue;
			
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

	DOREPLIFETIME_CONDITION( UR4Skill_PlayerCombo, ComboSkillAnimInfo, COND_OwnerOnly );
}

/**
 *  스킬 입력 시작
 */
void UR4Skill_PlayerCombo::OnInputStarted()
{
	// 스킬이 사용 가능하고, Combo Input Check가 불가능할 때 ( Combo Anim이 활성화 되지 않았다는 뜻 )
	// Combo Skill 시작
	if ( CanActivateSkill() && !CachedCanComboInput )
	{
		// Anim Play.
		PlaySkillAnim( ComboSkillAnimInfo );
		return;
	}
	
	// Server에서 Combo Skill Anim 사용 중이 확인되고, Combo Input Check가 가능하며,
	// Combo Input이 입력 된 상태가 아니면 요청
	if ( IsSkillAnimPlaying( ComboSkillAnimInfo.SkillAnimServerKey ) && CachedCanComboInput
		&& !CachedOnComboInput )
	{
		CachedOnComboInput = true;
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
	Super::OnBeginSkillAnim( InInstanceID, InSkillAnimInfo );
	
	// Anim Play 시작 성공 시 Combo Skill 사용중으로 판단
	if ( InSkillAnimInfo.SkillAnimServerKey == ComboSkillAnimInfo.SkillAnimServerKey )
	{
		// InputTest Notify <-> InputTest Bind
		_BindNotifiesAndInputTest( InInstanceID );
		CachedCanComboInput = true;
		CachedOnComboInput = false;
	}
}

/**
 *  Anim 종료 시 호출. Server와 Owner Client 에서 호출
 * @param InInstanceID : Play시 부여된 MontageInstanceID
 * @param InSkillAnimInfo : End될 Skill Anim 정보
 */
void UR4Skill_PlayerCombo::OnEndSkillAnim( int32 InInstanceID, const FR4SkillAnimInfo& InSkillAnimInfo, bool InIsInterrupted )
{
	Super::OnEndSkillAnim( InInstanceID, InSkillAnimInfo, InIsInterrupted );

	if ( InSkillAnimInfo.SkillAnimServerKey == ComboSkillAnimInfo.SkillAnimServerKey )
	{
		// InputTest Notify <-> InputTest Unbind
		_UnbindNotifiesAndInputTest( InInstanceID );
		CachedCanComboInput = false;
		CachedOnComboInput = false;

		// Combo Skill의 경우, 종료 시점에 SetCoolTime
		SetSkillCoolDownTime( GetSkillCoolDownTime( false ) );
	}
}

/**
 *  Server RPC의 Play Skill Anim 시 요청 무시 check에 사용.
 *  ( Validate Check 후 Server RPC 내에서 체크함으로 Index가 유효함이 보장 )
 */
bool UR4Skill_PlayerCombo::PlaySkillAnim_Ignore( uint32 InSkillAnimKey ) const
{
	// 이미 Anim을 Play 중이거나 || 스킬을 사용할 수 없을 때.
	if ( InSkillAnimKey == ComboSkillAnimInfo.SkillAnimServerKey )
		return Super::PlaySkillAnim_Ignore( InSkillAnimKey ) || !CanActivateSkill();
	
	return true;
}

/**
 * Server로 ComboInput을 요청
 */
void UR4Skill_PlayerCombo::_ServerRPC_RequestComboInput_Implementation()
{
	// 현재 Combo Input을 받을 수 있는 경우, true
	if ( CachedCanComboInput )
		CachedOnComboInput = true;
}

/**
 * Combo Input Test
 * Input Test 시점 이전에 입력이 들어와 있으면 (Server_CachedOnComboInput) 다음 Anim으로 Transition
 * @param InNotifyNumber : Input Test를 진행하라고 알린 Notify의 Index
 */
void UR4Skill_PlayerCombo::_ComboInputTest( uint8 InNotifyNumber )
{
	// Input Test가 지나고는 받을 수 없음. 성공적으로 Transition 시에는 다시 설정.
	CachedCanComboInput = false;
	
	if ( !CachedOnComboInput )
		return;

	// Combo Input Notify 정보를 Notify 기준으로 Sort해 놓았으므로 binary_search
	int32 index = Algo::LowerBound( ComboInputInfo, InNotifyNumber,
		[](const FR4ComboInputInfo& InElem1, int32 InIndex)
		{
			return InElem1.NotifyNumber < InIndex; 
		});
	
	// 다음 Section이 잘 존재하는지 확인
	if ( !ComboInputInfo.IsValidIndex( index ) || ComboInputInfo[index].NotifyNumber != InNotifyNumber ) 
		return;

	IR4AnimationInterface* owner = Cast<IR4AnimationInterface>(GetOwner());
	if ( owner == nullptr )
	{
		LOG_WARN( R4Skill, TEXT("Can only play Skill Animations if the IR4AnimationInterface is inherited.") )
		return;
	}
	
	// Section Transition In Local
	if ( GetOwnerRole() == ROLE_AutonomousProxy )
		owner->JumpToSection_Local( ComboInputInfo[index].NextSectionName );
	
	// Section Transition Server
	if ( GetOwnerRole() == ROLE_Authority )
		owner->Server_JumpToSection_WithoutAutonomous( ComboInputInfo[index].NextSectionName, true );

	CachedCanComboInput = true;
	CachedOnComboInput = false;
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
