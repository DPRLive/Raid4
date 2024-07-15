// Fill out your copyright notice in the Description page of Project Settings.


#include "R4CharacterBase.h"
#include "R4CharacterRPCComponent.h"
#include "R4CharacterRow.h"
#include "../Stat/R4StatComponent.h"
#include "../Movement/R4CharacterMovementComponent.h"
#include "../Skill/R4SkillComponent.h"
#include "../Skill/R4SkillBase.h"
#include "../Damage/R4DamageControlComponent.h"
#include "../UI/StatusBar/R4StatusBarWidget.h"

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

	StatComp = CreateDefaultSubobject<UR4StatComponent>(TEXT("StatComp"));

	SkillComp = CreateDefaultSubobject<UR4SkillComponent>(TEXT("SkillComp"));

	RPCComp = CreateDefaultSubobject<UR4CharacterRPCComponent>(TEXT("RPCComp"));

	DamageControlComp = CreateDefaultSubobject<UR4DamageControlComponent>(TEXT("DamageControlComp"));
}

/**
 *  PostInit
 */
void AR4CharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitStatComponent();
	
	OnCharacterDeadDelegate.AddDynamic(this, &AR4CharacterBase::Dead);
}

/**
 *  begin play
 */
void AR4CharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Character 테스트를 위한 Aurora 데이터 임시 로드
	// TODO : 나중에 캐릭터에 따른 데이터 로드를 진행해야함.
	PushDTData(1);
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
	
	if (!HasAuthority())
		return;

	///// Only Server /////
	
	// 스탯 컴포넌트에 데이터 입력
	StatComp->PushDTData(characterData->BaseStatRowPK);

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
 *  Damage를 처리한다. 음수의 데미지는 처리되지 않음.
 *  @param InInstigator : 가해자
 *  @param InDamage : 입힐 데미지
 */
void AR4CharacterBase::ReceiveDamage(AActor* InInstigator, float InDamage)
{
	// 최종적으로 받을 데미지를 계산
	DamageControlComp->PushNewDamage(InDamage);
	float calculatedDamage = DamageControlComp->GetCalculatedDamage();

	// TODO : barrier를 흠. 흐음..

	// StatComp에 적용
	float damagedHp = FMath::Clamp(StatComp->GetCurrentHp() - calculatedDamage, 0.f, StatComp->GetCurrentHp());
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
		statusBar->UpdateTotalHp(StatComp->GetBaseHp() + StatComp->GetModifierHp());
		statusBar->UpdateCurrentHp(StatComp->GetCurrentHp());
		
		// 총 체력 변경시 호출
		StatComp->OnChangeHp().AddWeakLambda(statusBar, [statusBar](float InBaseHp, float InModifierHp)
		{
			statusBar->UpdateTotalHp(InBaseHp + InModifierHp);
		});

		// 현재 체력 변경 시 호출
		StatComp->OnChangeCurrentHp().AddWeakLambda(statusBar, [statusBar](float InCurrentHp)
		{
			statusBar->UpdateCurrentHp(InCurrentHp);
		});
	}
}

/**
 *  Replicate를 거쳐서 anim을 play
 *  @return : AnimMontage의 링크를 포함한 특정 Section에 대한 시간
 */
float AR4CharacterBase::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	return RPCComp->PlayAnim(AnimMontage, StartSectionName, InPlayRate);
}

/**
 *  Replicate를 거쳐서 anim을 Stop
 */
void AR4CharacterBase::StopAnimMontage(UAnimMontage* AnimMontage)
{
	RPCComp->StopAllAnim();
}

/**
 *  StatComp와 필요한 초기화를 진행한다
 */
void AR4CharacterBase::InitStatComponent()
{
	StatComp->InitStats();
	
	// TODO : Bind Stats
	StatComp->OnChangeMovementSpeed().AddUObject(this, &AR4CharacterBase::ApplyMovementSpeed); // 이동속도 설정 바인드
}

/**
 *  이동 속도를 적용한다.
 */
void AR4CharacterBase::ApplyMovementSpeed(float InBaseMovementSpeed, float InModifierMovementSpeed) const
{
	// 이동 속도를 변경한다.
	if(UR4CharacterMovementComponent* moveComp = GetCharacterMovement<UR4CharacterMovementComponent>(); IsValid(moveComp))
	{
		moveComp->SetMaxWalkSpeed(InBaseMovementSpeed + InModifierMovementSpeed);
	}
}

/**
 *  죽음을 처리한다.
 */
void AR4CharacterBase::Dead()
{
	LOG_WARN(LogTemp, TEXT("DEAD"));
}
