#include "CharacterRow.h"
#include "../../../Character/CharacterBase.h"
#include "../../../Manager/DataTableManager.h"
#include "../../../Component/R4StatComponent.h"
#include "../../../Component/R4SkillComponent.h"
#include "../../../Skill/R4SkillBase.h"

#include <Components/SkeletalMeshComponent.h>
#include <Engine/SkeletalMesh.h>
#include <Animation/AnimInstance.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterRow)

GENERATE_DT_PTR_CPP( FCharacterRow, CharacterRow );

/**
 * 캐릭터가 데이터를 로드해 갈 수 있도록 한다.
 */
void FCharacterRow::LoadDataToCharacter(ACharacterBase* InCharacter) const
{
    if(InCharacter == nullptr)
    {
	    LOG_WARN( R4Data, TEXT("InCharacter is nullptr.") )
    	return;
    }

	USkeletalMeshComponent* meshComp = InCharacter->GetMesh();
	if(meshComp == nullptr)
		return;

	// 스켈레탈 메시 설정
	if(USkeletalMesh* skelMesh = LoadSoftObjectSync<USkeletalMesh>(SkeletalMesh))
		meshComp->SetSkeletalMesh(skelMesh);

	// 애니메이션 설정
	if(AnimInstance != nullptr)
		meshComp->SetAnimInstanceClass(AnimInstance);

    if (!InCharacter->HasAuthority())
    	return;
	
	///// Only Server /////
	
    // 스탯 컴포넌트에 기본 스탯을 적용.
    if (UR4StatComponent* statComp = InCharacter->FindComponentByClass<UR4StatComponent>())
    {
	    statComp->Server_SetBaseStat(BaseStatRowPK);
    }
    
    // 스킬 컴포넌트에 스킬을 적용.
	if (UR4SkillComponent* skillComp = InCharacter->FindComponentByClass<UR4SkillComponent>())
	{
		for (const TPair<ESkillIndex, TSubclassOf<UR4SkillBase>>& skill : Skills)
		{
			if (UR4SkillBase* instanceSkill = NewObject<UR4SkillBase>(InCharacter, skill.Value))
			{
				instanceSkill->RegisterComponent();
				skillComp->Server_AddSkill(skill.Key, instanceSkill);
			}
		}
	}
    
}
