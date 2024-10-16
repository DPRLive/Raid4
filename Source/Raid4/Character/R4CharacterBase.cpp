// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterBase.h"
#include "../Data/Character/R4CharacterRow.h"
#include "../Data/Character/R4CharacterSrcRow.h"
#include "../Stat/CharacterStat/R4CharacterStatComponent.h"
#include "../Movement/R4CharacterMovementComponent.h"
#include "../Skill/R4SkillComponent.h"
#include "../Skill/R4SkillBase.h"
#include "../Buff/R4BuffManageComponent.h"
#include "../Shield/R4ShieldComponent.h"
#include "../UI/R4WidgetComponent.h"
#include "../UI/Status/R4HpBarWidget.h"
#include "../UI/Status/R4NameplateWidget.h"
#include "../Damage/R4DamageStruct.h"
#include "../Animation/R4AnimationComponent.h"
#include "../Util/UtilStat.h"
#include "../Animation/R4AnimInstance.h"

#include <Components/SkeletalMeshComponent.h>
#include <Components/CapsuleComponent.h>
#include <Engine/SkeletalMesh.h>
#include <Components/WidgetComponent.h>


#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CharacterBase)

/**
 *  생성자, Move Comp를 R4 Character Movement Component 로 변경
 */
AR4CharacterBase::AR4CharacterBase(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer.SetDefaultSubobjectClass<UR4CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	StatComp = CreateDefaultSubobject<UR4CharacterStatComponent>( TEXT( "StatComp" ) );

	SkillComp = CreateDefaultSubobject<UR4SkillComponent>( TEXT( "SkillComp" ) );

	BuffManageComp = CreateDefaultSubobject<UR4BuffManageComponent>( TEXT( "BuffManageComp" ) );

	ShieldComp = CreateDefaultSubobject<UR4ShieldComponent>( TEXT( "ShieldComp" ) );

	AnimComp = CreateDefaultSubobject<UR4AnimationComponent>( TEXT( "AnimComp" ) );

	StatusBarComp = CreateDefaultSubobject<UR4WidgetComponent>( TEXT( "StatusBarComp" ) );
	StatusBarComp->SetWidgetSpace( EWidgetSpace::Screen );
	StatusBarComp->SetCollisionProfileName( Collision::G_ProfileNoCollision );
	StatusBarComp->SetHiddenInGame( true ); // 시작 시 hidden
	
	// Mesh NoCollision
	if( GetMesh() )
		GetMesh()->SetCollisionProfileName( Collision::G_ProfileNoCollision );

	// Requested Move Acceleration
	if( GetCharacterMovement() )
		GetCharacterMovement()->bRequestedMoveUseAcceleration = true;

	CachedCharacterDTKey = DTConst::G_InvalidPK;
	bDead = false;

	// Capsule
#if WITH_EDITOR
	if ( GetCapsuleComponent() )
	{
		// DEBUG
		GetCapsuleComponent()->ShapeColor = FColor::Turquoise;
		GetCapsuleComponent()->SetLineThickness( 3.f );
		GetCapsuleComponent()->SetHiddenInGame( false );
	}
#endif WITH_EDITOR
}

void AR4CharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Stat Comp binds
	BindStatComponent();
}

/**
 *  Delegate Clear
 */
void AR4CharacterBase::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	OnCharacterDeadDelegate.Clear();
	OnCharacterDamagedDelegate.Clear();
	
	Super::EndPlay( EndPlayReason );
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
void AR4CharacterBase::PlayAnimSync( UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate, float InStartServerTime )
{
	if( IsValid( AnimComp ) )
		AnimComp->PlayAnimSync( InAnimMontage, InStartSectionName, InPlayRate, InStartServerTime );
}

/**
 * 주어진 Montage의 FAnimMontageInstance를 반환
 * @param InMontage : 확인할 Montage
 * @return : 현재 해당 Montage가 Play되고 있지 않다면 nullptr
 */
FAnimMontageInstance* AR4CharacterBase::GetActiveInstanceForMontage( const UAnimMontage* InMontage ) const
{
	UAnimInstance* animInstance = IsValid( GetMesh() ) ? GetMesh()->GetAnimInstance() : nullptr;
	if ( IsValid( animInstance ) )
		return animInstance->GetActiveInstanceForMontage( InMontage );

	return nullptr;
}

/**
 * Montage Instance Clear 시 알림.
 */
FOnClearMontageInstance* AR4CharacterBase::OnClearMontageInstance()
{
	UAnimInstance* animInstance = IsValid( GetMesh() ) ? GetMesh()->GetAnimInstance() : nullptr;
	if ( UR4AnimInstance* r4AnimInstance = Cast<UR4AnimInstance>( animInstance ) )
		return &r4AnimInstance->OnClearMontageInstanceDelegate;

	return nullptr;
}

/**
 *  주어진 Character Data PK로 데이터를 읽어 초기화
 *  @param InPk : Character DT의 primary key
 */
void AR4CharacterBase::PushDTData( FPriKey InPk )
{
	const FR4CharacterRowPtr characterData( GetWorld(), InPk );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterData is Invalid. PK : [%d]"), InPk );
		return;
	}

	// Get Resource Pk
	const FR4CharacterSrcRowPtr characterSrcRow( GetWorld(), characterData->ResourceRowPK );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterSrcData is Invalid. PK : [%d]"), InPk );
		return;
	}

	// Cached Key
	CachedCharacterDTKey = InPk;
	
	// Capsule
	if( UCapsuleComponent* capsuleComp = GetCapsuleComponent() )
	{
		capsuleComp->SetCapsuleHalfHeight( characterData->CapsuleHalfHeight );
		capsuleComp->SetCapsuleRadius( characterData->CapsuleRadius );
	}

	// Mesh & Anim
	if ( USkeletalMeshComponent* meshComp = GetMesh() )
	{
		// Skeletal Mesh
		if ( USkeletalMesh* skelMesh = characterSrcRow->SkeletalMesh.LoadSynchronous(); IsValid( skelMesh ) )
			meshComp->SetSkeletalMesh( skelMesh );

		meshComp->SetRelativeTransform( characterSrcRow->MeshTransform );
		// 이거 해줘야됨!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		BaseTranslationOffset = characterSrcRow->MeshTransform.GetLocation();
		BaseRotationOffset = characterSrcRow->MeshTransform.GetRotation();
		
		// Anim
		meshComp->SetAnimInstanceClass( characterData->AnimInstance );

		AnimComp->SetDeadAnim( characterSrcRow->DeadAnim.LoadSynchronous() );
	}
	
	// Stat Data
	if( IsValid( StatComp ) )
		StatComp->PushDTData( characterData->BaseStatRowPK );

	// Set Status Widget
	if ( IsValid( StatusBarComp ) )
	{
		StatusBarComp->SetHiddenInGame( false );
		StatusBarComp->SetWidgetClass( characterData->StatusBarClass );
		StatusBarComp->SetDrawSize( characterData->StatusBarDrawSize );
		StatusBarComp->SetRelativeLocation( characterData->StatusBarRelativeLocation );
		StatusBarComp->AttachToComponent( GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket::G_HealthBarSocket );
	}
	
	if ( !HasAuthority() )
		return;
	
	///// Only Server /////
	
	// Add Skills
	for ( int32 idx = 0; idx < characterData->Skills.Num(); idx++ )
	{
		if ( UR4SkillBase* instanceSkill = NewObject<UR4SkillBase>( this, characterData->Skills[idx] );
			IsValid( instanceSkill ) )
		{
			instanceSkill->RegisterComponent();
			SkillComp->Server_AddSkill( idx, instanceSkill );
		}
	}
}

/**
 *  Dt로 Push한 Data를 Clear.
 */
void AR4CharacterBase::ClearDTData()
{
	// 초기값으로 돌리기 위한 CDO 준비
	const UObject* cdo = IsValid( GetClass() ) ? GetClass()->GetDefaultObject( true ) : nullptr;
	const AR4CharacterBase* characterCDO = Cast<AR4CharacterBase>( cdo );

	if( IsValid( characterCDO ) )
	{
		// Capsule
		UCapsuleComponent* capsuleComp = GetCapsuleComponent();
		UCapsuleComponent* capsuleCompCDO = characterCDO->GetCapsuleComponent();
		
		if( IsValid( capsuleComp ) && IsValid( capsuleCompCDO ) )
		{
			capsuleComp->SetCapsuleHalfHeight( capsuleCompCDO->GetUnscaledCapsuleHalfHeight() );
			capsuleComp->SetCapsuleRadius( capsuleCompCDO->GetUnscaledCapsuleRadius() );
		}

		// Mesh & Anim
		USkeletalMeshComponent* meshComp = GetMesh();
		USkeletalMeshComponent* meshCompCDO = characterCDO->GetMesh();
		if ( IsValid( meshComp ) && IsValid( meshCompCDO ) )
		{
			meshComp->SetSkeletalMesh( meshCompCDO->GetSkeletalMeshAsset() );
			meshComp->SetRelativeTransform( meshCompCDO->GetRelativeTransform() );
			// 이거 해줘야됨!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			BaseTranslationOffset = meshCompCDO->GetRelativeTransform().GetLocation();
			BaseRotationOffset = meshCompCDO->GetRelativeTransform().GetRotation();
		
			// Anim
			meshComp->SetAnimInstanceClass( meshCompCDO->GetAnimClass() );
		}
	}
	
	// Anim Comp
	if( IsValid( AnimComp ) )
		AnimComp->SetDeadAnim( nullptr );
	
	// Status Widget
	if ( IsValid( StatusBarComp ) )
	{
		StatusBarComp->SetHiddenInGame( true );
		StatusBarComp->SetWidgetClass( nullptr );
		StatusBarComp->SetDrawSize( FVector2D::ZeroVector );
		StatusBarComp->SetRelativeLocation( FVector::ZeroVector );
		StatusBarComp->DetachFromComponent( FDetachmentTransformRules::KeepRelativeTransform );
	}
	
	// Stat
	if( IsValid( StatComp ) )
		StatComp->ClearDTData();
	
	if ( !HasAuthority() )
		return;
	
	///// Only Server /////
	
	// Add Skills
	if( IsValid( SkillComp ) )
		SkillComp->ClearSkills();
}

/**
 *  데미지를 입는 함수. 음수의 데미지는 처리하지 않음.
 *  @param InInstigator : 데미지를 가한 가해자 액터
 *  @param InDamageInfo : 데미지에 관한 정보.
 */
void AR4CharacterBase::ReceiveDamage(AActor* InInstigator, const FR4DamageReceiveInfo& InDamageInfo)
{
	if( !IsValid( StatComp ) )
		return;
	
	// 음수의 데미지를 수신한 경우 경고
	if(InDamageInfo.IncomingDamage < 0.f)
		LOG_WARN(R4Data, TEXT("[%s] : Receive negative damage."), *GetName());
	
	float reducedDamage = InDamageInfo.IncomingDamage;
	
	// 방어력 적용
	reducedDamage *= UtilStat::GetDamageReductionFactor(StatComp->GetTotalArmor());

	// 받는 피해 증감량 적용
	reducedDamage *= StatComp->GetTotalReceiveDamageMultiplier();

	// 방어막 적용
	if ( IsValid( ShieldComp ) )
		reducedDamage = reducedDamage - ShieldComp->ConsumeShield(reducedDamage);
	
	// 실제 HP 감소, StatComp에 적용
	float damagedHp = FMath::Max(StatComp->GetCurrentHp() - reducedDamage, 0.f);
	StatComp->SetCurrentHp(damagedHp);
}

/**
*  Buff를 받는 함수.
*  @param InInstigator : 버프를 시전한 액터
*  @param InBuffClass : 버프 클래스
*  @param InBuffSettingDesc : 버프 세팅 정보
*/
void AR4CharacterBase::ReceiveBuff(AActor* InInstigator, const TSubclassOf<UR4BuffBase>& InBuffClass, const FR4BuffSettingDesc& InBuffSettingDesc)
{
	// BuffComp에게 넘겨준다.
	if( IsValid( BuffManageComp ) )
		BuffManageComp->AddBuff(InInstigator, InBuffClass, InBuffSettingDesc);
}

/**
 *  Tag에 맞는 FR4StatInfo 형태의 Stat을 반환.
 */
FR4StatInfo* AR4CharacterBase::GetStatByTag(const FGameplayTag& InTag) const
{
	if( IsValid( StatComp ) )
		return StatComp->GetStatByTag<FR4StatInfo>( InTag );

	return nullptr;
}

/**
 *  Tag에 맞는 FR4CurrentStatInfo 형태의 Stat 반환.
 */
FR4CurrentStatInfo* AR4CharacterBase::GetCurrentStatByTag(const FGameplayTag& InTag) const
{
	if( IsValid( StatComp ) )
		return StatComp->GetStatByTag<FR4CurrentStatInfo>( InTag );

	return nullptr;
}

/**
 *  HP bar setup
 */
void AR4CharacterBase::SetupHpBarWidget( UUserWidget* InWidget )
{
	// Bind HP bar
	if ( UR4HpBarWidget* hpBar = Cast< UR4HpBarWidget >( InWidget ) )
	{
		// 초기화
		if( IsValid( StatComp ) )
		{
			hpBar->SetTotalHp( StatComp->GetTotalHp() );
			hpBar->SetCurrentHp( StatComp->GetCurrentHp() );

			// 총 체력 변경시 호출
			StatComp->OnChangeHp().AddWeakLambda( hpBar, [hpBar]( float InPrevTotalHp, float InNowTotalHp )
			{
				hpBar->SetTotalHp( InNowTotalHp );
			} );

			// 현재 체력 변경 시 호출
			StatComp->OnChangeCurrentHp().AddWeakLambda( hpBar, [hpBar]( float InPrevCurrentHp, float InNowCurrentHp )
			{
				hpBar->SetCurrentHp( InNowCurrentHp );
			} );
		}
		
		if( IsValid( ShieldComp ) )
		{
			hpBar->SetCurrentShieldAmount( ShieldComp->GetTotalShield() );
		
			// 방어막 변경 시 호출
			ShieldComp->OnChangeTotalShieldDelegate.AddWeakLambda( hpBar, [hpBar]( float InNowShieldAmount )
			{
				hpBar->SetCurrentShieldAmount( InNowShieldAmount );
			} );
		}
	}
}

/**
 *  Nameplate Widget Setup. 기본적으로 Character DT Key에 포함된 Character Name을 사용.
 */
void AR4CharacterBase::SetupNameplateWidget( UUserWidget* InWidget )
{
	const FR4CharacterRowPtr characterData( GetWorld(), CachedCharacterDTKey );
	if ( !characterData.IsValid() )
	{
		LOG_ERROR( R4Data, TEXT("CharacterData is Invalid. PK : [%d]"), CachedCharacterDTKey );
		return;
	}
	
	UR4NameplateWidget* nameplate = Cast<UR4NameplateWidget>( InWidget );
	if( IsValid( nameplate ) )
		nameplate->SetName( characterData->CharacterName );
}

/**
 *  StatComp와 필요한 바인딩을 진행
 */
void AR4CharacterBase::BindStatComponent()
{
	if( !IsValid( StatComp ) )
		return;
	
	// Bind Stats
	StatComp->OnChangeMovementSpeed().AddUObject( this, &AR4CharacterBase::ApplyMovementSpeed ); // 이동속도 설정 바인드

	// Dead Bind
	StatComp->OnChangeCurrentHp().AddWeakLambda(this, 
[this]( float InPrevHp, float InNowHp )
		{
			if ( !bDead && InNowHp < KINDA_SMALL_NUMBER )
			{
				if ( OnCharacterDeadDelegate.IsBound() )
					OnCharacterDeadDelegate.Broadcast( this );
				Dead();
			}
		});

	// Damage Delegate Bind
	StatComp->OnChangeCurrentHp().AddWeakLambda(this, 
[this]( float InPrevHp, float InNowHp )
	    {
	    	if ( InNowHp < InPrevHp )
	    	{
	    		if ( OnCharacterDamagedDelegate.IsBound() )
	    			OnCharacterDamagedDelegate.Broadcast( ( InPrevHp - InNowHp ) );
	    	}
	    });
}

/**
 *  이동 속도를 적용
 */
void AR4CharacterBase::ApplyMovementSpeed( float InPrevMovementSpeed, float InNowMovementSpeed ) const
{
	// 이동 속도를 변경
	if ( UR4CharacterMovementComponent* moveComp = GetCharacterMovement<UR4CharacterMovementComponent>() )
		moveComp->MaxWalkSpeed = InNowMovementSpeed;
}

/**
 *  죽음을 처리
 */
void AR4CharacterBase::Dead()
{
	bDead = true;

	if ( IsValid( AnimComp ) )
		AnimComp->PlayDeadAnim();

	// Buff, Shield 모두 해제
	if( IsValid( BuffManageComp ) )
		BuffManageComp->ClearBuffs();

	if( IsValid( ShieldComp ) )
		ShieldComp->ClearShields();

	// Hide StatusBar
	if( IsValid( StatusBarComp ) )
		StatusBarComp->SetHiddenInGame( true );
	
	// 이동 제한
	if( UCharacterMovementComponent* moveComp = GetCharacterMovement() )
		moveComp->SetMovementMode( MOVE_None );

	// UR4CharacterMovementComponent 사용 시 ForceMove Clear
	if( UR4CharacterMovementComponent* moveComp = GetCharacterMovement<UR4CharacterMovementComponent>() )
		moveComp->ClearForceMove();
	
	// not collision, Physics simulate 사용 안해서 바닥으로 안떨어짐.
	SetActorEnableCollision( false );
}