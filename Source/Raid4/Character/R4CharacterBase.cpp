// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterBase.h"
#include "R4CharacterRow.h"
#include "../Stat/CharacterStat/R4CharacterStatComponent.h"
#include "../Movement/R4CharacterMovementComponent.h"
#include "../Skill/R4SkillComponent.h"
#include "../Skill/R4SkillBase.h"
#include "../Buff/R4BuffManageComponent.h"
#include "../Shield/R4ShieldComponent.h"
#include "../UI/StatusBar/R4StatusBarWidget.h"
#include "../Damage/R4DamageStruct.h"
#include "../Animation/R4AnimationComponent.h"
#include "../Util/UtilStat.h"
#include "../Animation/R4AnimInstance.h"

#include <Components/SkeletalMeshComponent.h>
#include <Engine/SkeletalMesh.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4CharacterBase)

/**
 *  생성자, Move Comp를 R4 Character Movement Component 로 변경
 */
AR4CharacterBase::AR4CharacterBase(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer.SetDefaultSubobjectClass<UR4CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	StatComp = CreateDefaultSubobject<UR4CharacterStatComponent>(TEXT("StatComp"));

	SkillComp = CreateDefaultSubobject<UR4SkillComponent>(TEXT("SkillComp"));

	BuffManageComp = CreateDefaultSubobject<UR4BuffManageComponent>(TEXT("BuffManageComp"));
	
	ShieldComp = CreateDefaultSubobject<UR4ShieldComponent>(TEXT("ShieldComp"));
	
	AnimComp = CreateDefaultSubobject<UR4AnimationComponent>(TEXT("AnimComp"));

	// Mesh NoCollision
	GetMesh()->SetCollisionProfileName(Collision::G_ProfileNoCollision);

	bDead = false;
}

/**
 *  PostInit
 */
void AR4CharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 컴포넌트간 연결
	BindStatComponent();
	
	OnCharacterDeadDelegate.AddDynamic(this, &AR4CharacterBase::Dead);
}

/**
 *  begin play
 */
void AR4CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	// TODO : 데이터 집어넣는건 PlayerController가 Character PK를 들고 있다가 OnPossess 와 OnRep_Owner 되면 넣는걸로 하면 될 듯
	// Character 테스트를 위한 Aurora 데이터 임시 로드
	PushDTData(1);
	
	// test
	if(HasAuthority())
	{
		for(auto& [buffClass, desc] : TestingBuffs)
			BuffManageComp->AddBuff(this, buffClass, desc);
	}
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
 *  주어진 Character Data PK로 데이터를 읽어 초기화한다.
 *  @param InPk : Character DT의 primary key
 */
void AR4CharacterBase::PushDTData(FPriKey InPk)
{
	const FR4CharacterRowPtr characterData(InPk);
	if(!characterData.IsValid())
	{
		LOG_ERROR(R4Data, TEXT("CharacterData is Invalid. PK : [%d]"), InPk);
		return;
	}
	
	if(USkeletalMeshComponent* meshComp = GetMesh(); IsValid(meshComp))
	{
		// 스켈레탈 메시 설정
		if(USkeletalMesh* skelMesh = characterData->SkeletalMesh.LoadSynchronous(); IsValid(skelMesh))
			meshComp->SetSkeletalMesh(skelMesh);

		// 애니메이션 설정
		meshComp->SetAnimInstanceClass(characterData->AnimInstance);
	}

	// 스탯 컴포넌트에 데이터 입력
	StatComp->PushDTData(characterData->BaseStatRowPK);
	
	if (!HasAuthority())
		return;
	
	///// Only Server /////

	// 스킬 컴포넌트에 스킬을 적용.
	// TODO : 배열 주면 Skill Comp에서 읽어가게 하는게 좋을거 같단말이야
	for ( int32 idx = 0; idx < characterData->Skills.Num(); idx++ )
	{
		if (UR4SkillBase* instanceSkill = NewObject<UR4SkillBase>(this, characterData->Skills[idx]); IsValid(instanceSkill))
		{
			instanceSkill->RegisterComponent();
			SkillComp->Server_AddSkill( idx, instanceSkill);
		}
	}
}

/**
 *  데미지를 입는 함수. 음수의 데미지는 처리하지 않음.
 *  @param InInstigator : 데미지를 가한 가해자 액터
 *  @param InDamageInfo : 데미지에 관한 정보.
 */
void AR4CharacterBase::ReceiveDamage(AActor* InInstigator, const FR4DamageReceiveInfo& InDamageInfo)
{
	// 음수의 데미지를 수신한 경우 경고
	if(InDamageInfo.IncomingDamage < 0.f)
		LOG_WARN(R4Data, TEXT("[%s] : Receive negative damage."), *GetName());
	
	float reducedDamage = InDamageInfo.IncomingDamage;
	
	// 방어력 적용
	reducedDamage *= UtilStat::GetDamageReductionFactor(StatComp->GetTotalArmor());

	// 받는 피해 증감량 적용
	reducedDamage *= StatComp->GetTotalReceiveDamageMultiplier();

	// 방어막 적용
	reducedDamage = reducedDamage - ShieldComp->ConsumeShield(reducedDamage);
	
	// 실제 HP 감소, StatComp에 적용
	float damagedHp = FMath::Max(StatComp->GetCurrentHp() - reducedDamage, 0.f);
	StatComp->SetCurrentHp(damagedHp);

	// Damage 수신 알림
	if( OnCharacterDamagedDelegate.IsBound() )
		OnCharacterDamagedDelegate.Broadcast( InInstigator, reducedDamage );
	
	// 죽었다면 죽었다고 알림
	if(FMath::IsNearlyZero(damagedHp) && OnCharacterDeadDelegate.IsBound())
		OnCharacterDeadDelegate.Broadcast();
}

/**
 *  Status bar를 Setup
 */
void AR4CharacterBase::SetupStatusBarWidget(UUserWidget* InWidget)
{
	if(UR4StatusBarWidget* statusBar = Cast<UR4StatusBarWidget>(InWidget); IsValid(statusBar))
	{
		// 초기화
		statusBar->UpdateTotalHp(StatComp->GetTotalHp());
		statusBar->UpdateCurrentHp(StatComp->GetCurrentHp());
		statusBar->UpdateCurrentShieldAmount(ShieldComp->GetTotalShield());
		
		// 총 체력 변경시 호출
		StatComp->OnChangeHp().AddWeakLambda(statusBar, [statusBar](float InTotalHp)
		{
			statusBar->UpdateTotalHp(InTotalHp);
		});

		// 현재 체력 변경 시 호출
		StatComp->OnChangeCurrentHp().AddWeakLambda(statusBar, [statusBar](float InCurrentHp)
		{
			statusBar->UpdateCurrentHp(InCurrentHp);
		});

		// 방어막 변경 시 호출
		ShieldComp->OnChangeTotalShieldDelegate.AddWeakLambda(statusBar, [statusBar](float InShieldAmount)
		{
			statusBar->UpdateCurrentShieldAmount(InShieldAmount);
		});
	}
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
	BuffManageComp->AddBuff(InInstigator, InBuffClass, InBuffSettingDesc);
}

/**
 *  Tag에 맞는 FR4StatInfo 형태의 Stat을 반환.
 */
FR4StatInfo* AR4CharacterBase::GetStatByTag(const FGameplayTag& InTag) const
{
	return StatComp->GetStatByTag<FR4StatInfo>( InTag );
}

/**
 *  Tag에 맞는 FR4CurrentStatInfo 형태의 Stat 반환.
 */
FR4CurrentStatInfo* AR4CharacterBase::GetCurrentStatByTag(const FGameplayTag& InTag) const
{
	return StatComp->GetStatByTag<FR4CurrentStatInfo>( InTag );
}

/**
 *  StatComp와 필요한 바인딩을 진행
 */
void AR4CharacterBase::BindStatComponent()
{
	// Bind Stats
	StatComp->OnChangeMovementSpeed().AddUObject(this, &AR4CharacterBase::ApplyMovementSpeed); // 이동속도 설정 바인드
}

/**
 *  이동 속도를 적용한다.
 */
void AR4CharacterBase::ApplyMovementSpeed(float InMovementSpeed) const
{
	// 이동 속도를 변경한다.
	if(UR4CharacterMovementComponent* moveComp = GetCharacterMovement<UR4CharacterMovementComponent>(); IsValid(moveComp))
	{
		moveComp->MaxWalkSpeed = InMovementSpeed;
	}
}

/**
 *  죽음을 처리한다.
 */
void AR4CharacterBase::Dead()
{
	// TODO : Set Collision, Hide widget, stat, skill reset 등 해야함, Respawn을 위한 reset 기능도 추가
	LOG_WARN(LogTemp, TEXT("DEAD"));

	bDead = true;
}
