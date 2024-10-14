#pragma once

#include "../../Data/R4RowBase.h"
#include "R4CharacterSrcRow.generated.h"

class UAnimMontage;
class USkeletalMesh;

/**
 * Character에 적용할 Resource 정보들을 담는 Row. 
 */
USTRUCT()
struct FR4CharacterSrcRow : public FR4RowBase
{
	GENERATED_BODY()

	FR4CharacterSrcRow()
	: SkeletalMesh( nullptr )
	, MeshTransform( FTransform::Identity )
	, CharacterPickedAnim ( nullptr )
	, DeadAnim ( nullptr )
	{}
	
	// 캐릭터에 사용할 Skeletal Mesh 
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Mesh" )
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	// Skeletal Mesh Transform
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Mesh" )
	FTransform MeshTransform;

	// 캐릭터 선택 시 Play 할 Anim
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Anim" )
	TSoftObjectPtr<UAnimMontage> CharacterPickedAnim;
	
	// 캐릭터 사망 모션. ( Loop로 설정할 것 )
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Anim" )
	TSoftObjectPtr<UAnimMontage> DeadAnim;
};

GENERATE_DT_PTR_H( FR4CharacterSrcRow );