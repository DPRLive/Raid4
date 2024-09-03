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
#include "../Util/UtilDamage.h"
#include "../Util/UtilAnimation.h"

#include <Components/SkeletalMeshComponent.h>
#include <Engine/SkeletalMesh.h>
#include <Animation/AnimInstance.h>

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
			BuffManageComp->Server_AddBuff(this, buffClass, desc);
	}
}

/**
 *  Local에서 Anim Play
 */
float AR4CharacterBase::PlayAnim_Local(UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate)
{
	if(!IsValid(InAnimMontage))
		return 0.0f;
	
	PlayAnimMontage(InAnimMontage, InPlayRate, InStartSectionName);

	int32 sectionIndex = InAnimMontage->GetSectionIndex(InStartSectionName);

	if(sectionIndex == INDEX_NONE) // INDEX가 NONE이면 시작 Section Index를 설정
		sectionIndex= InAnimMontage->GetSectionIndexFromPosition(0);
	
	return UtilAnimation::GetCompositeAnimLength(InAnimMontage, sectionIndex);
}

/**
 *  Local에서 Anim Stop
 */
void AR4CharacterBase::StopAnim_Local()
{
	StopAnimMontage(nullptr);
}

/**
 *  Server에서, Autonomous Proxy를 제외하고 AnimPlay를 명령. ServerTime 조정으로 동기화 가능.
 */
float AR4CharacterBase::Server_PlayAnim_WithoutAutonomous(UAnimMontage* InAnimMontage, const FName& InStartSectionName, float InPlayRate, bool InIsWithServer, float InServerTime)
{
	return AnimComp->Server_PlayAnim_WithoutAutonomous(InAnimMontage, InStartSectionName, InPlayRate, InIsWithServer, InServerTime);
}

/**
 *  Server에서, Autonomous Proxy를 제외하고 AnimStop을 명령.
 */
void AR4CharacterBase::Server_StopAnim_WithoutAutonomous(bool InIsWithServer)
{
	AnimComp->Server_StopAnim_WithoutAutonomous(InIsWithServer);
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
	for (const TPair<ESkillIndex, TSubclassOf<UR4SkillBase>>& skill : characterData->Skills)
	{
		if (UR4SkillBase* instanceSkill = NewObject<UR4SkillBase>(this, skill.Value); IsValid(instanceSkill))
		{
			instanceSkill->RegisterComponent();
			SkillComp->Server_AddSkill(skill.Key, instanceSkill);
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
	reducedDamage *= UtilDamage::CalculateReductionByArmor(StatComp->GetTotalArmor());

	// 받는 피해 증감량 적용
	reducedDamage *= StatComp->GetTotalReceiveDamageMultiplier();

	// 방어막 적용
	reducedDamage = reducedDamage - ShieldComp->ConsumeShield(reducedDamage);
	
	// 실제 HP 감소, StatComp에 적용
	float damagedHp = FMath::Clamp(StatComp->GetCurrentHp() - reducedDamage, 0.f, StatComp->GetCurrentHp());
	StatComp->SetCurrentHp(damagedHp);
	
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
	if(GetLocalRole() == ROLE_Authority)
		BuffManageComp->Server_AddBuff(InInstigator, InBuffClass, InBuffSettingDesc);
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
}
