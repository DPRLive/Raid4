// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "../Component/R4StatComponent.h"
#include "../Data/DataTable/Row/CharacterRow.h"
#include "../Component/R4CharacterMovementComponent.h"
#include "../Component/R4SkillComponent.h"

#include "Raid4/Skill/R4SkillBase.h"

#include <Components/SkeletalMeshComponent.h>
#include <Engine/SkeletalMesh.h>
#include <Animation/AnimInstance.h>

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
	_InitCharacterData(1);
	
}

/**
 *  begin play
 */
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  주어진 스탯 Data로 스탯을 초기화한다.
 *  @param InCharacterDataPk : Character DT의 primary key
 */
void ACharacterBase::_InitCharacterData(FPriKey InCharacterDataPk)
{
	const FCharacterRowPtr characterData(InCharacterDataPk);
	if(!characterData.IsValid())
	{
		LOG_ERROR(R4Data, TEXT("CharacterData is Invalid. PK : [%d]"), InCharacterDataPk);
		return;
	}
	
	if(USkeletalMeshComponent* meshComp = GetMesh())
	{
		// 스켈레탈 메시 설정
		if(USkeletalMesh* skelMesh = characterData->SkeletalMesh.LoadSynchronous())
			meshComp->SetSkeletalMesh(skelMesh);

		// 애니메이션 설정
		meshComp->SetAnimInstanceClass(characterData->AnimInstance);
	}
	
	if (!HasAuthority())
		return;

	///// Only Server /////
	
	// 스탯 초기화
	_InitStatData(characterData->BaseStatRowPK);

	// 스킬 컴포넌트에 스킬을 적용.
	// TODO : 배열 주면 Skill Comp에서 읽어가게 하는게 좋을거 같단말이야
	for (const TPair<ESkillIndex, TSubclassOf<UR4SkillBase>>& skill : characterData->Skills)
	{
		if (UR4SkillBase* instanceSkill = NewObject<UR4SkillBase>(this, skill.Value))
		{
			instanceSkill->RegisterComponent();
			SkillComp->Server_AddSkill(skill.Key, instanceSkill);
		}
	}
}

/**
 *  주어진 스탯 Data로 스탯을 초기화한다.
 *  @param InStatPk : Stat DT의 primary key
 */
void ACharacterBase::_InitStatData(FPriKey InStatPk)
{
	// TODO : Bind Stats
	// 이동속도 설정 바인드
	// TODO : Server / Client 나눠야함. 이렇게 위에서 저기서 호출하면 CLient가 bind를 못함
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