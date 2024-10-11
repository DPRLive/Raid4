// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Skill_PlayerCombo.h"
#include "../../Util/UtilAnimation.h"

#include <Net/UnrealNetwork.h>
#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Skill_PlayerCombo)

UR4Skill_PlayerCombo::UR4Skill_PlayerCombo()
{
	SetIsReplicatedByDefault( true );

	CachedCanComboInput = false;
	CachedOnComboInput = false;
	CachedIsComboInputTestPassed = false;
}

#if WITH_EDITOR
void UR4Skill_PlayerCombo::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
	Super::PostEditChangeProperty( PropertyChangedEvent );

	if(PropertyChangedEvent.MemberProperty == nullptr)
		return;
	
	// 변경된 프로퍼티가 FSkillAnimInfo 형식이면, 해당 Anim에서 Section을 읽어와 배열을 자동으로 채움.
	if ( FStructProperty* prop = CastField<FStructProperty>( PropertyChangedEvent.MemberProperty );
		prop != nullptr &&
		prop->Struct == FR4SkillAnimInfo::StaticStruct() )
	{
		FR4SkillAnimInfo* animInfo = prop->ContainerPtrToValuePtr<FR4SkillAnimInfo>( this );
		UAnimMontage* anim = animInfo->SkillAnim;

		if ( !IsValid( anim ) )
			return;
		
		// Section 찾기
		TSet<FName> nowSections;
		for ( int32 idx = 0; idx < anim->GetNumSections(); idx++ )
		{
			FName sectionName = anim->GetSectionName( idx );
			nowSections.Emplace( sectionName );
		}

		// 필요 없는 Section 제거
		for ( auto it = ComboInputInfo.CreateIterator(); it; ++it )
		{
			if( !nowSections.Contains( it->NowSectionName ) )
				it.RemoveCurrent();
		}

		// 필요한 Section 추가
		for( const auto& sectionName : nowSections )
		{
			auto elem = ComboInputInfo.FindByPredicate( [&sectionName]( const FR4ComboInputInfo& InComboInput )
			{
				return InComboInput.NowSectionName == sectionName;
			} );
			
			if( elem == nullptr )
				ComboInputInfo.Emplace( FR4ComboInputInfo( anim->GetSectionIndex( sectionName ), sectionName ) );
		}

		// SORT
		ComboInputInfo.Sort( []( const FR4ComboInputInfo& InElem1, const FR4ComboInputInfo& InElem2 )
		{
			return InElem1.NowSectionIndex < InElem2.NowSectionIndex;
		} );
	}
}
#endif

void UR4Skill_PlayerCombo::GetLifetimeReplicatedProps( TArray<class FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	// Anim Key를 Replicate 받을 수 있도록 설정
	DOREPLIFETIME_CONDITION( UR4Skill_PlayerCombo, ComboSkillAnimInfo, COND_InitialOnly );
}

/**
 *  스킬 입력 시작
 */
void UR4Skill_PlayerCombo::OnInputStarted()
{
	if ( !CanActivateSkill() )
		return;
	
	// Combo Input Check가 불가능할 때 ( Combo Anim이 활성화 되지 않았다는 뜻 )
	// Combo Skill 시작
	if ( !CachedCanComboInput )
	{
		// Anim Play Start
		bool bPlayAnim = PlaySkillAnim_WithServerRPC( ComboSkillAnimInfo, NAME_None );

		if ( bPlayAnim )
		{
			CachedCanComboInput = true;
			
			// Skill 사용 시작 판정
			if( OnBeginSkill.IsBound() )
				OnBeginSkill.Broadcast();
		}
		return;
	}
	
	// Server에서 Combo Skill Anim 사용 중이 확인되고, Combo Input Check가 가능하며,
	// Combo Input이 입력 된 상태가 아니면 요청
	if ( IsSkillAnimServerPlaying( ComboSkillAnimInfo.SkillAnimServerKey ) && CachedCanComboInput && !CachedOnComboInput )
	{
		CachedOnComboInput = true;
		_ServerRPC_RequestComboInput();
	}
}

/**
 * Anim을 Play시작 시 호출.
 * @param InSkillAnimInfo : Play된 Skill Anim 정보
 * @param InStartSectionName : 시작 된 Section Name.
 * @param InStartServerTime : Skill Anim이 시작된 Server Time
 */
void UR4Skill_PlayerCombo::OnBeginSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, const FName& InStartSectionName, float InStartServerTime )
{
	Super::OnBeginSkillAnim( InSkillAnimInfo, InStartSectionName, InStartServerTime );

	// server & autonomous only
	if( GetOwnerRole() == ROLE_SimulatedProxy )
		return;
	
	// Anim Play 시작 성공 시 Combo Skill 사용중으로 판단, Combo Input 가능
	if ( InSkillAnimInfo.SkillAnimServerKey == ComboSkillAnimInfo.SkillAnimServerKey )
	{
		if ( !IsValid( InSkillAnimInfo.SkillAnim ) )
		{
			LOG_WARN( R4Skill, TEXT("Skill Anim is Invalid. Check SkillAnim Replicate State.") );
			return;
		}

		int32 nowSectionIndex = InSkillAnimInfo.SkillAnim->GetSectionIndex( InStartSectionName );
		nowSectionIndex = ( nowSectionIndex == INDEX_NONE ) ? 0 : nowSectionIndex;
		
		// Combo Input Test 정보를 SectionIndex 기준으로 Sort해 놓았으므로 binary_search
		int32 index = Algo::LowerBound( ComboInputInfo, nowSectionIndex,
			[]( const FR4ComboInputInfo& InElem1, int32 InIndex )
			{
				return InElem1.NowSectionIndex < InIndex; 
			});
		
		// Add Execute Combo Input Test
		// 다음 Section이 잘 존재하면, Add Execute
		if ( ComboInputInfo.IsValidIndex( index ) && ComboSkillAnimInfo.SkillAnim->IsValidSectionName( ComboInputInfo[index].NextSectionName ) )
		{
			CachedOnComboInput = false;
			CachedIsComboInputTestPassed = false;

			// Execute Delay Rate는 Anim과 동기화
			float sectionLength = UtilAnimation::GetCompositeAnimLength( InSkillAnimInfo.SkillAnim, nowSectionIndex );
			float executeDelayRate = CalculateDelayRate( sectionLength, InStartServerTime );

			// delay가 너무 길어졌으면 Skip
			if ( executeDelayRate < 0.f )
			{
				LOG_N( R4Skill, TEXT("Execute delay is too late. Anim Section Length : [%f], delay : [%f]"), sectionLength, R4GetServerTimeSeconds( GetWorld() ) - InStartServerTime );
				return;
			}

			AddInputTestExecute( ComboSkillAnimInfo.SkillAnimServerKey, ComboInputInfo[index], executeDelayRate );
		}
	}
}

/**
 * Anim 종료 시 호출.
 * @param InSkillAnimInfo : End된 Skill Anim 정보
 */
void UR4Skill_PlayerCombo::OnEndSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo )
{
	Super::OnEndSkillAnim( InSkillAnimInfo );
	
	// 아닌 경우 종료.
	if ( InSkillAnimInfo.SkillAnimServerKey == ComboSkillAnimInfo.SkillAnimServerKey )
	{
		// Input Test를 성공 한 상태에서 종료 시 Section이 Transition된 것으로 판단.
		if( CachedIsComboInputTestPassed )
			return;
		
		if ( OnEndSkill.IsBound() )
			OnEndSkill.Broadcast();
		
		// server & autonomous only
		if( GetOwnerRole() == ROLE_SimulatedProxy )
			return;
	
		CachedCanComboInput = false;
		CachedOnComboInput = false;

		// Combo Skill의 경우, 종료 시점에 SetCoolTime
		SetSkillCoolDownTime( GetSkillCoolDownTime( false ) );
	}
}

/**
 * Combo Input Test Execute 추가
 *  @param InSkillAnimKey : Execute가 필요한 Skill Anim Key
 *  @param InComboInputInfo : 현재 Combo Input 정보
 *  @param InDelayRate :Delay를 얼마나 빠르게 체크할지, ( 실제로 InDelay / InDelayRate 뒤에 실행 됨 ). InDelayRate < 0.f인 경우는 처리하지 않음 !
 */
void UR4Skill_PlayerCombo::AddInputTestExecute( int32 InSkillAnimKey, const FR4ComboInputInfo& InComboInputInfo, float InDelayRate )
{
	// InPlayRate가 음수이면, 처리하지 않음.
	if( InDelayRate < 0.f )
	{
		LOG_WARN( R4Skill, TEXT("Negative InDelayRate [%f] is not processed."), InDelayRate );
		return;
	}

	AddExecute( InSkillAnimKey, [thisPtr = TWeakObjectPtr<UR4Skill_PlayerCombo>(this), &InComboInputInfo]()
	{
		if ( thisPtr.IsValid() )
			thisPtr->_ComboInputTest( InComboInputInfo );
	}, InComboInputInfo.InputTestDelay, InDelayRate );
}


/**
 * Combo Input Test
 * Input Test 시점 이전에 입력이 들어와 있으면 ( CachedOnComboInput ) 다음 Anim으로 Transition
 * @param InComboInputInfo : 현재 Combo Input 정보
 */
void UR4Skill_PlayerCombo::_ComboInputTest( const FR4ComboInputInfo& InComboInputInfo )
{
	// server & autonomous only
	if( GetOwnerRole() == ROLE_SimulatedProxy )
		return;
	
	// Combo Input이 입력 되었는지?
	if ( !CachedOnComboInput )
	{
		CachedIsComboInputTestPassed = false;
		return;
	}
	
	// pass combo input test
	CachedIsComboInputTestPassed = true;
	
	// Section Transition, Server & Locally Control
	PlaySkillAnimSync_Local( ComboSkillAnimInfo, InComboInputInfo.NextSectionName, R4GetServerTimeSeconds( GetWorld() ) );
}

/**
 * Server로 ComboInput을 요청
 */
void UR4Skill_PlayerCombo::_ServerRPC_RequestComboInput_Implementation()
{
	CachedOnComboInput = true;
}
