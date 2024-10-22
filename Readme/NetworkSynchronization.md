
# 네트워크 동기화

- 여러 플레이어가 같은 게임 환경에서 일관된 경험을 유지 및 부정행위 방지를 위해 여러 네트워크 동기화 로직을 작성하였습니다.

## Animation Synchronization
- 이 프로젝트는 보스 몬스터와의 전투를 중심으로 진행되는 게임입니다. 보스의 스킬 모션을 보고 플레이어가 회피하거나 공격하는 것이 핵심이므로, 애니메이션 동기화가 매우 중요한 요소입니다.

- 하지만, 애니메이션을 재생할 때 Client -> Server -> 다른 플레이어들 또는 Server -> 다른 플레이어들로 패킷이 전송되는 구조로 인해 패킷 전송 지연이 발생할 수 있습니다. 이로 인해 애니메이션이 늦게 재생되어 다른 플레이어들이 보스의 모션을 늦게 보게 되는 문제가 발생할 수 있습니다.

- 따라서 이러한 지연 문제를 해결하고 정확하고 즉각적인 전투 경험을 제공하기 위해 애니메이션 동기화 작업을 진행하였습니다.

### UR4AnimationComponent
![image](https://github.com/user-attachments/assets/4c97475c-738e-4c8f-8716-3620da00ae3d)

- Anim의 플레이가 필요한 외부 객체는 IR4AnimationInterface를 통해서 애니메이션의 플레이를 요청하고, 필요 시 UR4AnimationComponent의 동기화 함수를 활용하여 동기화된 애니메이션 플레이가 가능하도록 하였습니다. 

- UR4AnimationComponent의 애니메이션의 동기화는 Anim의 전체시간과 ServerTimeSecond를 기준으로 이루어지며, 세 경우로 나누어집니다.
    
- Anim의 전체 시간은 DFS를 통해 Anim Montage의 Link Section을 포함한 총 실제 Play되는 길이를 구하여 이용하였으며, [[여기]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Util/UtilAnimation.cpp)에서 확인할 수 있습니다.

#### Loop Animation의 경우
- Anim을 시작한 ServerTime과 현재 ServerTime을 비교하여 Delay를 구한 후, Animation을 시작 지점으로부터 Delay 된 지점에서 시작

#### 일반 Animation의 경우
- 동일 시점에 애니메이션이 끝나도록 PlayRate를 (Anim의 전체시간 / 시작한 ServerTime을 기준으로 하여 남은시간)으로 설정하여 보정

#### 지연 시간이 Animation의 총 길이보다 긴 경우
- 스킵

[[R4AnimationComponent.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Animation/R4AnimationComponent.cpp)

```
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

```

## Skill
- Skill에서 주로 사용되는 피격 판정, 애니메이션 등을 위한 동기화 및 부정행위 방지 로직을 작성하였습니다.

### Skill Animation
- UR4AnimSkillBase를 상속하여 확장한 다양한 동작 방식의 Skill 클래스들은 Animation의 Begin과 End를 통해 Skill의 사용 여부를 판단하는 경우가 많습니다.

- 또한, UR4AnimSkillBase는 Skill Animation을 Play 요청하는 로직이 존재해야 하며, 이는 Server뿐만 아니라 Client Side에서 요청이 진행되기 때문에 Validation Check도 진행해야 하였습니다.

- 우선 Skill Animation을 Client와 Server 사이에서 구분할 수 있도록, Server에서 클래스 멤버로 존재하는 FR4SkillAnimInfo (Skill에서 사용할 Animation과 부가 정보를 담은 구조체)에 Key를 할당하였습니다.

- Anim Key 할당 시, 클래스를 상속하여 확장 후 Skill Animation을 얼마나 사용할 지 예측할 수 없고, FR4SkillAnimInfo를 멤버로 일일이 추가시 누락하는 실수를 방지할 수 있도록 리플렉션을 통해 파싱하였습니다.

- 뿐만 아니라, Server에서의 Skill Anim 상태를 확인할 수 있도록 Skill Anim Key 크기 만큼의 배열 AnimPlayServerStates를 Replicate 하여 각 Skill Anim Key 별로 실행이 시작 된 시간, Anim Montage Section Index, MontageInstanceID 등의 정보를 Replicate하여 동기화, 요청 제한 등에 사용하였습니다.

[[R4AnimSkillBase.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Skill/R4AnimSkillBase.cpp)

```
/**
 *  Skill Anim 멤버를 파싱
 *  Server에서는 Key값 부여도 진행
 */
void UR4AnimSkillBase::_ParseSkillAnimInfo()
{
	for ( const auto& [prop, value] : TPropertyValueRange<FStructProperty>( GetClass(), this ) )
	{
		if ( prop->Struct != FR4SkillAnimInfo::StaticStruct() )
			continue;
		
		const FR4SkillAnimInfo* c_animInfoPtr = static_cast<const FR4SkillAnimInfo*>(value);
		if ( !IsValid(c_animInfoPtr->SkillAnim) )
			continue;

		// 키값 부여 & 캐싱
		if ( FR4SkillAnimInfo* animInfoPtr = const_cast<FR4SkillAnimInfo*>(c_animInfoPtr) )
		{
			// 서버일 시, Skill Anim Info에 키값을 부여 ( 0 ~ N )
			if ( GetOwnerRole() == ROLE_Authority )
				animInfoPtr->SkillAnimServerKey = CachedSkillAnimInfos.Num();
			
			CachedSkillAnimInfos.Emplace( c_animInfoPtr );
		}
	}

	// SkillAnimPlayState를 Skill Anim만큼 초기화
	if ( GetOwnerRole() == ROLE_Authority )
		AnimPlayServerStates.SetNum( CachedSkillAnimInfos.Num() );
}
```

- 해당 Key를 통해 Client에서 Server로 Skill Anim Play를 요청할 수 있으며, Server에서 RPC를 수신 시, 올바른 Key로 요청하는지 확인하여 Validation Check를 진행하였습니다.

- Validation 체크에 실패하면 플레이어가 추방당하게 되는데, 너무 과도한 Validation 체크가 진행되면 플레이어가 무고한 추방을 당할 수도 있기 때문에, 적당한 이상 패킷은 추방이 아닌 단순 '무시'를 할 수 있도록 Ignore함수를 추가하였습니다.

- 해당 Validation 체크 함수와 Ignore함수는 virtual 함수로, 클래스 확장 시 필요한 유효성 체크를 추가하여 진행할 수 있도록 하였습니다.

[[R4AnimSkillBase.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Skill/R4AnimSkillBase.cpp)

```
/**
 *  Server Animation Play를 요청의 유효성을 PlaySkillAnim_Validate로 확인.
 *  PlaySkillAnim_Validate() 을 Override하여 키에 따라 로직 작성
 *  @param InSkillAnimKey : Server에서 부여받은 FR4SkillAnimInfo의 Key.
 *  @param InStartSectionIndex : 시작한 Section Index
 *  @param InStartServerTime : Anim을 Play 시작한 ServerTime.
 */
bool UR4AnimSkillBase::_ServerRPC_PlaySkillAnim_Validate( int32 InSkillAnimKey, int32 InStartSectionIndex, float InStartServerTime )
{
	return PlaySkillAnim_Validate( InSkillAnimKey );
}

/**
 *  Server RPC의 Validation check 시 사용.
 *  기본적으로 유효한 Anim Server Key로 요청인지 확인
 */
bool UR4AnimSkillBase::PlaySkillAnim_Validate( int32 InSkillAnimKey ) const
{
	// 유효한 Server Key로 요청 시 Validate 성공
	return IsValidSkillAnimKey( InSkillAnimKey );
}

/**
 *  Server RPC의 Play Skill Anim 시 요청 무시 check에 사용.
 *  기본적으로 Skill을 사용할 수 있는 상태인지 확인
 */
bool UR4AnimSkillBase::PlaySkillAnim_Ignore( int32 InSkillAnimKey ) const
{
	return !CanActivateSkill();
}
```

- 위에서 할당받은 Anim Key를 이용하여 Client에서 우선 SkillAnim을 Play후 Server로 Skill Anim Play 요청을 할 수 있습니다.

- Skill Animation의 대략적인 Play 과정은 다음과 같습니다.
    - Client에서 Animation Play 후 RPC로 요청
    - Server에서 패킷 수신, Server에서 Anim Play 후 AnimPlayServerStates에 정보를 기록하여 Replicate
    - Simulated Proxy에서 Replicate된 AnimPlayServerStates를 확인하여 Animation을 Play.

- Animation의 Play는 위에서 설명한 Interface를 통해 Character를 거쳐 UR4AnimationComponent를 이용하여 동기화 하였습니다.

[[R4AnimSkillBase.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Skill/R4AnimSkillBase.cpp)

```
/**
 *  Skill Animation을 Local에서 Play. InStartServerTime으로 동기화 가능.
 *  멤버로 등록된 Skill Anim만 Server에서 Play 가능.
 *  @param InSkillAnimInfo : 멤버로 등록된, Play할 Skill Anim Info
 *  @param InStartSectionName : 시작 된 Section Name.
 *  @param InStartServerTime : Skill Anim이 시작 된 Server Time.
 *  @return : Play Anim 성공 여부
 */
bool UR4AnimSkillBase::PlaySkillAnimSync_Local( const FR4SkillAnimInfo& InSkillAnimInfo, const FName& InStartSectionName, float InStartServerTime )
{
	if ( !IsValidSkillAnimKey( InSkillAnimInfo.SkillAnimServerKey ) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimInfo.SkillAnimServerKey );
		return false;
	}

	if ( !ensureMsgf( IsValid( InSkillAnimInfo.SkillAnim ), TEXT("Skill Anim is nullptr.") ) )
		return false;
	
	IR4AnimationInterface* owner = Cast<IR4AnimationInterface>( GetOwner() );
	if ( owner == nullptr )
	{
		LOG_WARN( R4Skill, TEXT("Can only play Skill Animations if the IR4AnimationInterface is inherited.") )
		return false;
	}

	// 같은 Anim을 두번 사용하면, Blend Out 시점을 End Play Anim으로 판정하더라도
	// Begin 2 () -> End 1 () 순서로 로직이 진행되어 문제가 생김. ( Montage Instance는 파괴되지만, Anim은 queue에서 대기 )
	// 그래서 Montage Instance 파괴 시점을 Get
	FOnClearMontageInstance* montageInstanceDelegate = owner->OnClearMontageInstance();
	if( montageInstanceDelegate == nullptr )
	{
		LOG_WARN( R4Skill, TEXT("montageInstanceDelegate is nullptr.") )
		return false;
	}
	
	// Start Section Name이 invalid하면, 시작 Section으로 설정
	FName startSectionName = InStartSectionName;
	if( !InSkillAnimInfo.SkillAnim->IsValidSectionName( startSectionName ) )
		startSectionName = InSkillAnimInfo.SkillAnim->GetSectionName( 0 );
		
	// Local에서 Play
	owner->PlayAnimSync( InSkillAnimInfo.SkillAnim, startSectionName, 1.f, InStartServerTime );
	FAnimMontageInstance* montageInstance = owner->GetActiveInstanceForMontage( InSkillAnimInfo.SkillAnim );
	if ( montageInstance == nullptr )
	{
		LOG_ERROR( R4Skill, TEXT("FAnimMontageInstance is nullptr.") )
		return false;
	}

	// Anim Play End시 로직 설정, 
	// this capture, use weak lambda
	FDelegateHandle handle = montageInstanceDelegate->AddWeakLambda( this,
		[this, &InSkillAnimInfo, mID = montageInstance->GetInstanceID()]
		( FAnimMontageInstance& InStoppedMontageInstance )
			{
				if( InStoppedMontageInstance.GetInstanceID() != mID )
					return;
				
				IR4AnimationInterface* ownerInterface = Cast<IR4AnimationInterface>( GetOwner() );
				if ( ownerInterface == nullptr )
					return;
			
				FOnClearMontageInstance* mIDelegate = ownerInterface->OnClearMontageInstance();
				if( mIDelegate == nullptr )
					return;
	
				// END
				OnEndSkillAnim( InSkillAnimInfo );
			
				// UnBind Delegates..
				FDelegateHandle dHandle;
				CachedClearMontageHandles.RemoveAndCopyValue( mID, dHandle );
				mIDelegate->Remove( dHandle );
			} );

	// Add DelegateHandle
	CachedClearMontageHandles.Emplace( montageInstance->GetInstanceID(), MoveTemp( handle ) );
	OnBeginSkillAnim( InSkillAnimInfo, startSectionName, R4GetServerTimeSeconds( GetWorld() ) );

	return true;
}

/**
 *  Skill Animation을 Play 후 Server RPC 전송을 통해 동기화.
 *  멤버로 등록된 Skill Anim만 Server에서 Play 가능.
 *  @param InSkillAnimInfo : 멤버로 등록된, Play할 Skill Anim Info
 *  @param InStartSectionName : 시작 된 Section Name.
 *  @return : Play Anim 성공 여부
 */
bool UR4AnimSkillBase::PlaySkillAnim_WithServerRPC( const FR4SkillAnimInfo& InSkillAnimInfo, const FName& InStartSectionName )
{
	if ( !IsValidSkillAnimKey( InSkillAnimInfo.SkillAnimServerKey ) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimInfo.SkillAnimServerKey );
		return false;
	}

	if ( !ensureMsgf( IsValid( InSkillAnimInfo.SkillAnim ), TEXT("Skill Anim is nullptr.") ) )
		return false;

	// 현재 서버 시간.
	float nowServerTime = R4GetServerTimeSeconds( GetWorld() );

	// Start Section Name이 invalid하면, 시작 Section으로 설정
	FName startSectionName = InStartSectionName;
	if( !InSkillAnimInfo.SkillAnim->IsValidSectionName( startSectionName ) )
		startSectionName = InSkillAnimInfo.SkillAnim->GetSectionName( 0 );
	
	// ROLE_AutonomousProxy인 경우 로컬에서 먼저 플레이 후 Server RPC 전송
	if( GetOwnerRole() == ROLE_AutonomousProxy )
	{
		bool bPlayAnim = PlaySkillAnimSync_Local( InSkillAnimInfo, startSectionName, nowServerTime );

		if ( !bPlayAnim )
			return false;
	}

	// Server로 Play 요청
	int32 startIndex = InSkillAnimInfo.SkillAnim->GetSectionIndex( startSectionName );
	_ServerRPC_PlaySkillAnim( InSkillAnimInfo.SkillAnimServerKey, startIndex, nowServerTime );
	return true;
}

/**
 *  Server로 Animation Play를 요청.
 *  @param InSkillAnimKey : Server에서 부여받은 FR4SkillAnimInfo의 Key.
 *  @param InStartSectionIndex : 시작한 Section Index
 *  @param InStartServerTime : Anim을 Play 시작한 ServerTime.
 */
void UR4AnimSkillBase::_ServerRPC_PlaySkillAnim_Implementation( int32 InSkillAnimKey, int32 InStartSectionIndex, float InStartServerTime )
{
	// 특정 경우 요청을 무시.
	if ( PlaySkillAnim_Ignore( InSkillAnimKey ) )
		return;
	
	const FR4SkillAnimInfo* skillAnimInfo = GetSkillAnimInfo( InSkillAnimKey );
	if ( ( skillAnimInfo == nullptr )
		|| ( !ensureMsgf( IsValid( skillAnimInfo->SkillAnim ), TEXT("Skill Anim is nullptr.") ) ) )
		return;
	
	// Server Anim Play
	FName startSectionName = skillAnimInfo->SkillAnim->GetSectionName( InStartSectionIndex );
	PlaySkillAnimSync_Local( *skillAnimInfo, startSectionName, InStartServerTime );
}

/**
 *  Server에서 Play되는 Anim 상태가 변동 시 호출
 */
void UR4AnimSkillBase::_OnRep_AnimPlayServerState( const TArray<FAnimPlayServerStateInfo>& InPrevAnimPlayServerStates )
{
	// simulated proxy only.
	if( GetOwnerRole() != ROLE_SimulatedProxy
		|| InPrevAnimPlayServerStates.Num() != AnimPlayServerStates.Num() )
		return;
	
	// Simulated Proxy 일 시, Play에 맞춘 상황을 호출
	for ( int32 idx = 0; idx < AnimPlayServerStates.Num(); idx++ )
	{
		bool bPrevPlay = InPrevAnimPlayServerStates[idx].AnimStartServerTime > - KINDA_SMALL_NUMBER;
		bool bNowPlay = AnimPlayServerStates[idx].AnimStartServerTime > - KINDA_SMALL_NUMBER;
		bool bSectionChange = ( InPrevAnimPlayServerStates[idx].SectionIndex != AnimPlayServerStates[idx].SectionIndex );
		bool bMIDChange = ( InPrevAnimPlayServerStates[idx].MontageInstanceID != AnimPlayServerStates[idx].MontageInstanceID );
		
		const FR4SkillAnimInfo* animInfo = GetSkillAnimInfo( idx );
		if ( animInfo == nullptr || !IsValid( animInfo->SkillAnim ) )
		{
			LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), idx );
			continue;
		}

		FName newSectionName = animInfo->SkillAnim->GetSectionName( AnimPlayServerStates[idx].SectionIndex );
		// 새로 Play가 시작 된 경우 || Section만 Change된 경우 || override play 인 경우
		if ( ( !bPrevPlay && bNowPlay )
			|| ( bPrevPlay && bNowPlay && ( bSectionChange || bMIDChange ) ) )
		{
			PlaySkillAnimSync_Local( *animInfo, newSectionName, AnimPlayServerStates[idx].AnimStartServerTime );
		}
	}
}
```

### Skill Execute
- Skill에서 Animation을 실행하고 일정 시간 뒤 작업을 예약하는 'Execute' 기능을 구현하였는데, Anim Notify에 의존하지 않는 구조로 설계되어 Animation이 동기화되어 PlayRate가 변경된다면 Execute가 실행되는 시간이 의도한 Animation의 시점과 일치하지 않을 수 있습니다.

- 따라서 Anim의 Length와, 시작 시간을 이용하여 Execute를 예약하는 시간에 대한 DelayRate를 보정하는 로직을 작성하고, AddExecute 시 보정되어 예약되도록 하였습니다.

[[R4AnimSkillBase.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Skill/R4AnimSkillBase.cpp)
```
/**
 *  Section의 Length와, 시작 시간이 주어졌을 때, 현재 서버타임과 비교하여 얼마나 빠르게 실행해야 서버와 동일하게 끝낼 수 있는지 delay 계산
 *  @param InTotalLength : 실행 시간 ( ex ) Anim Section의 Length
 *  @param InStartTime : 현재 ServerTime과 비교할 시작 시간
 *  @return : TotalLength < Delay일 시, -1.f 반환
 */
float UR4AnimSkillBase::CalculateDelayRate( float InTotalLength, float InStartTime )
{
	float delay = FMath::Max( KINDA_SMALL_NUMBER, ( R4GetServerTimeSeconds( GetWorld() ) - InStartTime ) );

	// Total Length가 음수일 경우, Loop라고 판단, 1로 실행.
	if( InTotalLength < 0 )
		return 1.f;
	
	// delay가 InTotalLength보다 길면 -1.f return
	if( delay > InTotalLength )
		return -1.f;
	
	// DelayRate = 전체시간 / 남은시간
	return ( InTotalLength / FMath::Max( ( InTotalLength - delay ), KINDA_SMALL_NUMBER ) );
}

/**
 *  Skill Anim Key에 맞는, 특정 시간 뒤에 Execute 예약 추가.
 *  멤버로 등록된 Skill Anim Key만 추가 가능.
 *  InFunc에 Update 배열에 무언가를 추가하는 로직 작성 시 추가 로직 반복에 의한 메모리 누수 주의.
 *  @param InSkillAnimKey : 멤버로 등록된, Play할 Skill Anim Info
 *  @param InFunc : 실행시킬 Lambda, R-value, this 캡쳐 시 weak 캡쳐할 것
 *  @param InDelay : 실행 Delay. ( Delay <= 0 이면, 바로 실행 됨 ) 
 *  @param InDelayRate : Delay를 얼마나 빠르게 체크할지, ( 실제로 InDelay / InDelayRate 뒤에 실행 됨 ). InDelayRate < 0.f인 경우는 처리하지 않음 !
 */
void UR4AnimSkillBase::AddExecute( int32 InSkillAnimKey, TFunction<void()>&& InFunc, float InDelay, float InDelayRate )
{
	if ( !IsValidSkillAnimKey(InSkillAnimKey) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimKey );
		return;
	}

	if( !InFunc )
	{
		LOG_WARN( R4Skill, TEXT("InFunc is invalid.") );
		return;
	}

	// InPlayRate가 음수이면, 처리하지 않음.
	if( InDelayRate < 0.f )
	{
		LOG_WARN( R4Skill, TEXT("Negative InDelayRate [%f] is not processed."), InDelayRate );
		return;
	}

	// division by zero 방지
	InDelayRate = FMath::Max( KINDA_SMALL_NUMBER, InDelayRate );
	
	// 실제로 적용할 delay 계산
	InDelay /= InDelayRate;

	// Delay가 너무 작으면, 그냥 바로 실행
	if ( InDelay <= KINDA_SMALL_NUMBER )
	{
		if( InFunc )
			InFunc();

		return;
	}
	
	// 아니면 Update 대기열에 넣고 Tick On
	PendingExecutes.Emplace
	(
		FR4AnimSkillExecuteInfo
		(
			InSkillAnimKey,
			_GetNowMontageInstanceID( InSkillAnimKey ),
			R4GetServerTimeSeconds( GetWorld() ) + InDelay,
			MoveTemp(InFunc)
		)
	);
	
	SetComponentTickEnabled( true );
}
```

### Skill Detect & Buff
- Skill에서 Detect를 진행하거나 Buff를 적용하는 로직이 존재합니다.

- 이 Detect와 Buff를 네트워크 환경에서 적절히 처리해야 하는데, Detect나 Buff의 경우 모두 Server에서 처리하면 안되며, Locally Control이나 Simulated 에서도 같이 처리되어야 하는 다양한 경우가 존재합니다. 

(예시)
- Stat을 수정하는 Buff : Server에서만 처리
- Skill 사용 시 Camera Boom 조정 효과 Buff : Locally Control에서만 처리
- Detect 처리 시, Detect 결과로 Particle 생성 등 시각적인 요소가 필요한 경우 : Server, Locally Control, Simulated 모두 처리
- Detect 처리 시, 시각적인 요소를 포함하지 않는 경우 : Server에서만 처리
...

- 위와 같은 다양한 상황을 처리하기 위해, bitflag를 두어 BP에서 확장 및 데이터 설정 시 어느 머신에서 처리할 지 같이 설정할 수 있도록 하였습니다.
[[R4Enum.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Common/R4Enum.h)

```
/**
 * Network 정책 설정 시 사용할 Bitflag
 */
UENUM( BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true") )
enum class ER4NetworkFlag : uint8
{
	None		=  0		UMETA( Hidden ),
	Server		= (1 << 0)	UMETA( DisplayName = "Server" ),
	Local		= (1 << 1)	UMETA( DisplayName = "Locally Control" ),
	Simulated 	= (1 << 2)	UMETA( DisplayName = "Simulated Client" ),
	Max			= (1 << 3)	UMETA( Hidden )
};
ENUM_CLASS_FLAGS(ER4NetworkFlag);
```

[[R4SkillStruct.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Skill/R4SkillStruct.h)

```
/**
 * Skill에서 진행할 탐지에 관한 정보.
 */
USTRUCT()
struct FR4SkillDetectInfo
{
	GENERATED_BODY()

	FR4SkillDetectInfo()
	: DetectClass( nullptr )
	, DetectorNetFlag( 0 )
	, DetectorOriginCalculator( nullptr )
	, bAttachToMesh( false )
	, MeshSocketName( NAME_None )
	, DetectDesc( FR4DetectDesc() )
	{}
	
	// 어떤 클래스로 Detect를 진행할 것인지?
	// Dummy가 필요한 경우 (bHasVisual) Detect Class를 Replicate 설정해야함! 
	UPROPERTY( EditAnywhere, meta = ( MustImplement = "/Script/Raid4.R4DetectorInterface" ) )
	TSubclassOf<AActor> DetectClass;
	
	// Detector의 Network Spawn 정책을 설정
	// Replicated Detector일 시 Server에서만 Spawn 해야함.
	// particle이 있으나 투사체 등 (시각적인 요소 + 위치 같이) 중요하지 않다면 굳이 Replicate 하지 않고 Local 생성하는걸 권장
	UPROPERTY( EditAnywhere, meta = ( Bitmask, BitmaskEnum = "/Script/Raid4.ER4NetworkFlag" ) )
	uint8 DetectorNetFlag;

	// Detector의 Relative Location의 기준이 될 Origin을 계산하는 Class.
	UPROPERTY( EditAnywhere, meta = ( MustImplement = "/Script/Raid4.R4OriginCalculatorInterface" ) )
	TSubclassOf<UObject> DetectorOriginCalculator;
	
	// Detector를 Mesh에 Attach할 것인지?
	UPROPERTY( EditAnywhere )
	uint8 bAttachToMesh:1;

	// Skeletal Mesh Socket Name
	UPROPERTY( EditAnywhere, meta = ( EditCondition = "bAttachToMesh", EditConditionHides ) )
	FName MeshSocketName;
	
	// 탐지에 관한 Parameter.
	UPROPERTY( EditAnywhere )
	FR4DetectDesc DetectDesc;
};

/**
 * Skill에서 사용할 Buff에 대한 정보.
 */
USTRUCT()
struct FR4SkillBuffInfo
{
	GENERATED_BODY()

	FR4SkillBuffInfo()
	: BuffNetFlag ( 0 )
	, BuffInfo( FR4BuffWrapper() )
	{}
	
	// Buff를 적용할 Network 정책을 설정
	UPROPERTY( EditAnywhere, meta = ( Bitmask, BitmaskEnum = "/Script/Raid4.ER4NetworkFlag" ) )
	uint8 BuffNetFlag;
	
	// 적용할 버프 정보
	UPROPERTY( EditAnywhere )
	FR4BuffWrapper BuffInfo;
};
```

<img src="https://github.com/user-attachments/assets/d675ecea-8096-483e-adb6-e00135f5528b" width="500" height="278"/>

- 이 bitflag는 R4AnimSkillBase에서 Execute 추가 시 구분하여 추가됩니다.

[[R4SkillBase.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Skill/R4SkillBase.cpp)

```
/**
 *  NetFlag와 현재 Comp의 Net 상태를 비교 ( ER4NetworkFlag )
 *  @param InNetFlag : 비교할 Net Flag 정보 ( ER4NetworkFlag )
 *  @return : NetFlag와 Match하여 현재 Comp의 Net 상태와 맞으면 true 리턴
 */
bool UR4SkillBase::IsMatchNetFlag( uint8 InNetFlag ) const
{
	// Parse Net Flag
	bool bLocalMatch = false;
	if ( APawn* owner = Cast<APawn>( GetOwner() ) )
		bLocalMatch = ( InNetFlag & static_cast<uint8>( ER4NetworkFlag::Local ) ) && owner->IsLocallyControlled();
	
	bool bServerMatch = ( InNetFlag & static_cast<uint8>( ER4NetworkFlag::Server) ) && ( GetOwnerRole() == ROLE_Authority );
	
	bool bSimulatedMatch = (InNetFlag & static_cast<uint8>( ER4NetworkFlag::Simulated ) ) && ( GetOwnerRole() == ROLE_SimulatedProxy );

	return ( bLocalMatch || bServerMatch || bSimulatedMatch );
}
```

[[R4AnimSkillBase.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Skill/R4AnimSkillBase.cpp)

```
/**
 *  Detect Execute Net Flag에 맞춰 추가
 *  @param InSkillAnimKey : Execute가 필요한 Skill Anim Key
 *  @param InTimeDetectInfo : Delay와 Detect 정보
 *  @param InDelayRate :Delay를 얼마나 빠르게 체크할지, ( 실제로 InDelay / InDelayRate 뒤에 실행 됨 ). InDelayRate < 0.f인 경우는 처리하지 않음 !
 */
void UR4AnimSkillBase::AddDetectExecute( int32 InSkillAnimKey, const FR4SkillTimeDetectWrapper& InTimeDetectInfo, float InDelayRate )
{
	// InPlayRate가 음수이면, 처리하지 않음.
	if( InDelayRate < 0.f )
	{
		LOG_WARN( R4Skill, TEXT("Negative InDelayRate [%f] is not processed."), InDelayRate );
		return;
	}

	// Replicated Detector일 시 Server에서만 Spawn 하는지 확인
	{
		// Parse Net Flag
		bool bReplicate = InTimeDetectInfo.DetectEffect.DetectorInfo.DetectClass.GetDefaultObject()->GetIsReplicated();
		bool bServerSpawn = ( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Server) );
		bool bLocalSpawn = ( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Local ) );
		bool bSimulatedSpawn = ( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Simulated ) );
		
		if ( !ensureMsgf( bReplicate ? ( bServerSpawn && !bLocalSpawn && !bSimulatedSpawn ) : true,
				TEXT("Replicated Detector must be spawned only on Server") ) )
			return;
	}
	
	// Net Flag가 일치하지 않으면 추가하지 않음.
	if ( !IsMatchNetFlag( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag ) )
		return;
	
	AddExecute( InSkillAnimKey, [thisPtr = TWeakObjectPtr<UR4AnimSkillBase>(this), &detectInfo = InTimeDetectInfo.DetectEffect]()
	{
		if ( thisPtr.IsValid() )
			thisPtr->ExecuteDetect( detectInfo );
	}, InTimeDetectInfo.DelayTime, InDelayRate );
}

/**
 *  buff Execute Net Flag에 맞춰 추가
 *  @param InSkillAnimKey : Execute가 필요한 Skill Anim Key
 *  @param InTimeBuffInfo : buff 정보
 *  @param InDelayRate :Delay를 얼마나 빠르게 체크할지, ( 실제로 InDelay / InDelayRate 뒤에 실행 됨 ). InDelayRate < 0.f인 경우는 처리하지 않음 !
 */
void UR4AnimSkillBase::AddBuffExecute( int32 InSkillAnimKey, const FR4SkillTimeBuffWrapper& InTimeBuffInfo, float InDelayRate )
{
	// InPlayRate가 음수이면, 처리하지 않음.
	if( InDelayRate < 0.f )
	{
		LOG_WARN( R4Skill, TEXT("Negative InDelayRate [%f] is not processed."), InDelayRate );
		return;
	}

	// Net Flag가 일치하지 않으면 추가하지 않음.
	if ( !IsMatchNetFlag( InTimeBuffInfo.SkillBuffInfo.BuffNetFlag ) )
		return;
	
	AddExecute( InSkillAnimKey, [thisPtr = TWeakObjectPtr<UR4AnimSkillBase>(this), &InTimeBuffInfo]()
	{
		if ( thisPtr.IsValid() )
		{
			// 나에게 버프 적용
			thisPtr->ApplySkillBuff( InTimeBuffInfo.SkillBuffInfo );
		}
	}, InTimeBuffInfo.DelayTime, InDelayRate );
}
```