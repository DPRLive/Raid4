// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "../Component/R4SkillManageComponent.h"
#include "../Component/R4StatManageComponent.h"
#include "../Data/DataTable/Row/CharacterRow.h"
#include "../Component/R4CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterBase)

/**
 *  생성자, Move Comp를 R4 Character Movement Component 로 변경
 */
ACharacterBase::ACharacterBase(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer.SetDefaultSubobjectClass<UR4CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	SkillManageComp = CreateDefaultSubobject<UR4SkillManageComponent>(TEXT("SkillManageComp"));

	StatManageComp = CreateDefaultSubobject<UR4StatManageComponent>(TEXT("StatManageComp"));
}

/**
 *  PostInit
 */
void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 스탯 변경시 적용한다.
	StatManageComp->OnChangeTotalStat.AddUObject(this, &ACharacterBase::_ApplyTotalStat);
	
	// Character 테스트를 위한 Yin 데이터 임시 로드
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
 *  현재 캐릭터가 스킬을 사용할 수 있는지 판단
 *  TODO : 추후 디버프, 상태이상 등을 확인
 */
bool ACharacterBase::CanActivateSkill()
{
	return true;
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
