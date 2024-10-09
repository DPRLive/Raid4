// Fill out your copyright notice in the Description page of Project Settings.


#include "R4NonPlayerCharacter.h"

#include "../../Controller/R4AIController.h"
#include "../../Skill/NonPlayer/R4NonPlayerSkillComponent.h"
// test..
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Raid4/Character/R4CharacterRow.h"
#include "Raid4/Skill/R4SkillBase.h"
#include "Raid4/Stat/CharacterStat/R4CharacterStatComponent.h"
#include "Engine/SkeletalMesh.h"
//

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4NonPlayerCharacter)

AR4NonPlayerCharacter::AR4NonPlayerCharacter( const FObjectInitializer& InObjectInitializer )
	: Super( InObjectInitializer.SetDefaultSubobjectClass<UR4NonPlayerSkillComponent>( FName( TEXT( "SkillComp" ) ) ) )
{
	PrimaryActorTick.bCanEverTick = false;

	MaxPatrolRadius = 800.f;
	AIRotationSpeed = 300.f;
	
	// AI Controller 지정
	AIControllerClass = AR4AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AR4NonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnCharacterDamagedDelegate.AddDynamic( this, &AR4NonPlayerCharacter::OnAICharacterDamaged );
}

/**
 *  스킬 사용 요청 처리
 *  @return : Skill 사용 성공 시, Skill의 종료를 알리는 Delegate
 */
FSimpleMulticastDelegate* AR4NonPlayerCharacter::ActivateAISkill( uint8 InSkillIndex )
{
	if ( UR4NonPlayerSkillComponent* skillComp = Cast<UR4NonPlayerSkillComponent>( SkillComp ) )
		return skillComp->ActivateAISkill( InSkillIndex );
	
	return nullptr;
}

/**
 *  사용 가능한 스킬 중, ActivateSkillMinDist가 큰 Skill Index를 반환
 */
int32 AR4NonPlayerCharacter::GetAvailableMaxDistSkillIndex( float& OutDist ) const
{
	if ( UR4NonPlayerSkillComponent* skillComp = Cast<UR4NonPlayerSkillComponent>( SkillComp ) )
		return skillComp->GetAvailableMaxDistSkillIndex( OutDist );

	return INDEX_NONE;
}

void AR4NonPlayerCharacter::PushDTData( FPriKey InPk )
{
	const FR4CharacterRowPtr characterData(100);
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
	for (const TPair<EPlayerSkillIndex, TSubclassOf<UR4SkillBase>>& skill : characterData->Skills)
	{
		if (UR4SkillBase* instanceSkill = NewObject<UR4SkillBase>(this, skill.Value); IsValid(instanceSkill))
		{
			instanceSkill->RegisterComponent();
			SkillComp->Server_AddSkill( static_cast<uint8>(skill.Key), instanceSkill);
		}
	}
}

/**
 *	AI Character가 Damage 수신 시 로직
 */
void AR4NonPlayerCharacter::OnAICharacterDamaged( const AActor* InInstigator, float InDamage )
{
	const AController* instigatorController = Cast<AController>( InInstigator );
	if( !IsValid( instigatorController ) )
	{
		// InInstigator가 Controller가 아닌 경우. APawn이라면 controller를 get
		if ( const APawn* instigator = Cast<APawn>( InInstigator ) )
			instigatorController = instigator->GetController();
	}

	if( !IsValid( instigatorController ) )
		return;

	float& totalDamage = CachedDamagedControllers.FindOrAdd( instigatorController );
	totalDamage += InDamage;
}