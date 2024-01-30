// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "../Component/R4StatComponent.h"
#include "../Data/DataTable/Row/CharacterRow.h"
#include "../Component/R4CharacterMovementComponent.h"
#include "../Component/R4SkillComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterBase)

/**
 *  생성자, Move Comp를 R4 Character Movement Component 로 변경
 */
ACharacterBase::ACharacterBase(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer.SetDefaultSubobjectClass<UR4CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	StatComp = CreateDefaultSubobject<UR4StatComponent>(TEXT("StatManageComp"));

	SkillComp = CreateDefaultSubobject<UR4SkillComponent>(TEXT("SkillComp"));
}

/**
 *  PostInit
 */
void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 스탯 변경시 적용한다.
	StatComp->OnChangeTotalStat.AddUObject(this, &ACharacterBase::_ApplyTotalStat);
	
	// Character 테스트를 위한 Aurora 데이터 임시 로드
	// TODO : 나중에 캐릭터에 따른 데이터 로드를 진행해야함.
	const FCharacterRowPtr characterData(1);
	if(!characterData.IsValid())
		return;
	
	characterData.GetRow()->LoadDataToCharacter(this);
}

/**
 *  begin play
 */
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  변경된 스탯을 적용한다.
 */
void ACharacterBase::_ApplyTotalStat(const FStatRow& InBaseStat, const FStatRow& InModifierStat)
{
	// 이동 속도를 변경한다.
	if(UR4CharacterMovementComponent* moveComp = Cast<UR4CharacterMovementComponent>(GetCharacterMovement()))
	{
		moveComp->SetMaxWalkSpeed(InBaseStat.MovementSpeed + InModifierStat.MovementSpeed);
	}
}