#pragma once

#include "../Data/R4RowBase.h"
#include "R4CharacterRow.generated.h"

class USkeletalMesh;
class UAnimInstance;
class UR4SkillBase;

/**
 * Character에 적용할 정보들을 담는 Row 
 */
USTRUCT()
struct FR4CharacterRow : public FR4RowBase
{
	GENERATED_BODY()

	FR4CharacterRow()
	: CapsuleHalfHeight( 88.f )
	, CapsuleRadius ( 34.f )
	, SkeletalMesh( nullptr )
	, MeshTransform( FTransform::Identity )
	, AnimInstance( nullptr )
	, BaseStatRowPK( DTConst::G_InvalidPK )
	{}

	// Capsule Half Height
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Collision" )
	float CapsuleHalfHeight;
	
	// Capsule Size
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Collision" )
	float CapsuleRadius;
	
	// 캐릭터에 사용할 Skeletal Mesh 
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Mesh" )
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	// Skeletal Mesh Transform
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Mesh" )
	FTransform MeshTransform;
	
	// 캐릭터에 사용할 Anim Instance
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Anim" )
	TSubclassOf<UAnimInstance> AnimInstance;

	// 해당 캐릭터의 기본 스탯 정보를 담은 Data table의 Pk
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	int32 BaseStatRowPK;

	// 해당 캐릭터가 사용할 Skill Set.
	// 순서대로 Skill Comp에 등록되므로, Player Skill 등록 시 순서에 주의.
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Skill" )
	TArray<TSubclassOf<UR4SkillBase>> Skills;
};

GENERATE_DT_PTR_H( FR4CharacterRow );