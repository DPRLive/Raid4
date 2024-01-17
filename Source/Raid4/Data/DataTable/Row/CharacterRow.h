#pragma once

#include "../R4RowBase.h"
#include "../../../Interface/R4CharacterDataInterface.h"
#include "CharacterRow.generated.h"

class USkeletalMesh;
class UAnimInstance;
class UR4SkillBase;

/**
 * Character에 적용할 정보들을 담는 Row 
 */
USTRUCT()
struct FCharacterRow : public FR4RowBase, public IR4CharacterDataInterface
{
	GENERATED_BODY()

	FCharacterRow() : BaseStatRowPK(DTConst::G_InvalidPK) {}

	// 캐릭터에 사용할 Skeletal Mesh 
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Mesh" )
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	// 캐릭터에 사용할 Anim Instance
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Anim" )
	TSubclassOf<UAnimInstance> AnimInstance;

	// 해당 캐릭터의 기본 스탯 정보를 담은 Data table의 Pk
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	int32 BaseStatRowPK;

	// 해당 캐릭터가 사용할 스킬들
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Skill" )
	TMap<ESkillIndex, TSubclassOf<UR4SkillBase>> Skills;
	
	// 캐릭터가 데이터를 로드해 갈 수 있도록 한다.
	virtual void LoadDataToCharacter(ACharacterBase* InCharacter) const override;
};

GENERATE_DT_PTR_H( FCharacterRow );