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

	StatComp = CreateDefaultSubobject<UR4StatComponent>(TEXT("StatComp"));

	SkillComp = CreateDefaultSubobject<UR4SkillComponent>(TEXT("SkillComp"));
}

/**
 *  PostInit
 */
void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Character 테스트를 위한 Aurora 데이터 임시 로드
	// TODO : 나중에 캐릭터에 따른 데이터 로드를 진행해야함.
	// TODO : 그냥 읽어오는식으로 변경 필요
	const FCharacterRowPtr characterData(1);
	if(!characterData.IsValid())
		return;
	
	characterData.GetRow()->LoadDataToCharacter(this);

	_InitStat(characterData->BaseStatRowPK);
}

/**
 *  begin play
 */
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  스탯을 초기화한다.
 */
void ACharacterBase::_InitStat(FPriKey InStatPk)
{
	// TODO : Bind Stats
	// 이동속도 설정 바인드
	StatComp->GetOnChangeMovementSpeed().AddUObject(this, &ACharacterBase::_ApplyMovementSpeed);
	
	StatComp->InitStat(InStatPk);
}

/**
 *  이동 속도를 적용한다.
 */
void ACharacterBase::_ApplyMovementSpeed(float InBaseMovementSpeed, float InModifierMovementSpeed) const
{
	// 이동 속도를 변경한다.
	if(UR4CharacterMovementComponent* moveComp = GetCharacterMovement<UR4CharacterMovementComponent>())
	{
		moveComp->SetMaxWalkSpeed(InBaseMovementSpeed + InModifierMovementSpeed);
	}
}