// Fill out your copyright notice in the Description page of Project Settings.


#include "R4AnimSkillBase.h"

#include "../Detect/R4NotifyDetectInterface.h"
#include "../Animation/R4AnimationInterface.h"

#include <Animation/AnimMontage.h>
#include <Animation/AnimNotifies/AnimNotify.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4AnimSkillBase)

UR4AnimSkillBase::UR4AnimSkillBase()
{
	PrimaryComponentTick.bCanEverTick = false;

	CachedSkillAnimInfoCount = 0;
}

#if WITH_EDITOR
void UR4AnimSkillBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.MemberProperty == nullptr)
		return;
	
	// 변경된 프로퍼티가 FSkillAnimInfo 형식이면, 해당 Anim에서 Notify를 읽어와 배열을 자동으로 채움. 하하 아주 편리하지?
	if(FStructProperty* prop = CastField<FStructProperty>(PropertyChangedEvent.MemberProperty);
		prop != nullptr &&
		prop->Struct == FR4SkillAnimInfo::StaticStruct())
	{
		FR4SkillAnimInfo* animInfo = prop->ContainerPtrToValuePtr<FR4SkillAnimInfo>(this);
		UAnimMontage* anim = animInfo->SkillAnim;
		
		if(!IsValid(anim))
			return;

		// Detect Notify의 index들을 찾아냄 
		TSet<int32> idxs;
		for(int32 i = 0; i < anim->Notifies.Num(); i++)
		{
			if(IR4NotifyDetectInterface* detectNotify = Cast<IR4NotifyDetectInterface>(anim->Notifies[i].Notify))
				idxs.Emplace(i);
		}

		// 필요 없는 index는 제거
		for(auto it = animInfo->DetectNotify.CreateIterator(); it; ++it)
		{
			if(idxs.Find(it->Key) == nullptr)
			{
				it.RemoveCurrent();
				continue;
			}
			idxs.Remove(it->Key);
		}

		// 기존에 없는 index는 추가
		for(const auto& idx : idxs)
		{
			if(animInfo->DetectNotify.Find(idx) == nullptr)
				animInfo->DetectNotify.Emplace(idx);
		}

		// 보기 편하게 sort
		animInfo->DetectNotify.KeySort([](const int32& idx1, const int32& idx2)
		{
			return idx1 < idx2;
		});
	}
}
#endif

void UR4AnimSkillBase::BeginPlay()
{
	Super::BeginPlay();

	//  DetectNotify <-> FR4SkillDetectEffectInfo Bind 
	_ParseSkillAnimInfo();
}

/**
 *  Skill Animation을 Play.
 *  멤버로 등록된 Skill Anim만 Server에서 Play 가능.
 *  @param InSkillAnimInfo : 멤버로 등록된, Play할 Skill Anim Info
 *  @return : Play Anim 성공 여부
 */
bool UR4AnimSkillBase::PlaySkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo )
{
	if ( !ensureMsgf( IsValid(InSkillAnimInfo.SkillAnim), TEXT("Skill Anim is nullptr.") ) )
		return false;

	if ( InSkillAnimInfo.SkillAnimServerKey == Skill::G_InvalidSkillAnimKey )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key Is Invalid. Check SkillAnim Replicate State.") );
		return false;
	}
	
	// Authority가 아닌 경우 로컬에서 플레이 후 Server RPC 전송
	if( GetOwnerRole() != ROLE_Authority )
	{
		IR4AnimationInterface* owner = Cast<IR4AnimationInterface>(GetOwner());

		if ( owner == nullptr )
		{
			LOG_WARN( R4Skill, TEXT("Can only play Skill Animations if the IR4AnimationInterface is inherited.") )
			return false;
		}

		owner->PlayAnim_Local( InSkillAnimInfo.SkillAnim, NAME_None, 1.f );
		OnBeginSkillAnim( InSkillAnimInfo.SkillAnimServerKey );
	}

	_ServerRPC_PlaySkillAnim( InSkillAnimInfo.SkillAnimServerKey );
	return true;
}

/**
 *  Server로 Animation Play를 요청.
 *  @param InSkillAnimKey : Server에서 부여받은 FR4SkillAnimInfo의 Key.
 */
void UR4AnimSkillBase::_ServerRPC_PlaySkillAnim_Implementation( uint32 InSkillAnimKey )
{
	auto it = Server_CachedSkillAnimKey.Find( InSkillAnimKey );
	if(it == nullptr)
	{
		LOG_WARN( R4Skill, TEXT("InSkillKey [%d] is invalid. check replicate state."), InSkillAnimKey );
		return;
	}

	if ( !ensureMsgf( it->IsValid(), TEXT("Skill Anim is nullptr.") ) )
		return;

	IR4AnimationInterface* owner = Cast<IR4AnimationInterface>( GetOwner() );
	if ( owner == nullptr )
	{
		LOG_WARN( R4Skill, TEXT("Can only play Skill Animations if the IR4AnimationInterface is inherited.") )
		return;
	}

	// Autonomous 제외한 나머지 Animation Play.
	owner->Server_PlayAnim_WithoutAutonomous( it->Get(), NAME_None, 1.f, true, R4GetServerTimeSeconds() );
	OnBeginSkillAnim( InSkillAnimKey );
}

/**
 *  Server Animation Play를 요청의 유효성을 IsLockPlaySkillAnim()로 확인.
 *  IsLockPlaySkillAnim() 을 Override하여 키에 따라 로직 작성
 *  @param InSkillAnimKey : Server에서 부여받은 FR4SkillAnimInfo의 Key.
 */
bool UR4AnimSkillBase::_ServerRPC_PlaySkillAnim_Validate( uint32 InSkillAnimKey )
{
	return ( InSkillAnimKey != Skill::G_InvalidSkillAnimKey )
			&& !IsLockPlaySkillAnim( InSkillAnimKey );
}

/**
 *  Skill Anim 멤버를 찾아서 DetectNotify <-> FR4SkillDetectEffectInfo를 Bind. 및 Skill Anim 키 부여
 */
void UR4AnimSkillBase::_ParseSkillAnimInfo()
{
	// bind DetectNotify <-> FR4SkillDetectEffectInfo
	for ( const auto& [prop, value] : TPropertyValueRange<FStructProperty>( GetClass(), this ) )
	{
		// FSkillAnimInfo 타입의 struct를 찾아서 bind
		if ( prop->Struct != FR4SkillAnimInfo::StaticStruct() )
			continue;
		
		const FR4SkillAnimInfo* c_animInfoPtr = static_cast<const FR4SkillAnimInfo*>(value);
		UAnimMontage* anim = c_animInfoPtr->SkillAnim;
		if ( !ensureMsgf( IsValid(anim), TEXT("Skill Anim is invalid.") ) )
			return;

		// Server의 경우 Skill Anim Info에 Key 값을 부여.
		if ( GetOwnerRole() == ROLE_Authority )
		{
			if ( FR4SkillAnimInfo* animInfoPtr = const_cast<FR4SkillAnimInfo*>(c_animInfoPtr) )
			{
				animInfoPtr->SkillAnimServerKey = ++CachedSkillAnimInfoCount;
				Server_CachedSkillAnimKey.Emplace( animInfoPtr->SkillAnimServerKey, anim );
			}
		}

		for ( const auto& [notifyIdx, detectEffectInfo] : c_animInfoPtr->DetectNotify )
		{
			if ( !anim->Notifies.IsValidIndex( notifyIdx ) )
				continue;

			_BindDetectNotifyAndEffect( anim->Notifies[notifyIdx].Notify, detectEffectInfo );
		}
	}
}

/**
 *  DetectNotify <-> FR4DetectEffectWrapper 연결
 *  @param InDetectNotify : 탐지 타이밍 알림을 전달할 객체
 *  @param InDetectEffectInfo : Skill에서 무언가 탐지하고 줄 영향
 */
void UR4AnimSkillBase::_BindDetectNotifyAndEffect(const TScriptInterface<IR4NotifyDetectInterface>& InDetectNotify, const FR4DetectEffectWrapper& InDetectEffectInfo)
{
	if( IR4NotifyDetectInterface* detectNotifyObj = InDetectNotify.GetInterface() )
	{
		// this 캡처, WeakLambda 사용
		detectNotifyObj->OnNotifyDetect( GetOwner() ).BindWeakLambda(this,
			[this, &InDetectEffectInfo]()
			{
				// 탐지 실행
				ExecuteDetect( InDetectEffectInfo );
			});
	}
}
