#pragma once

#include "../Data/R4RowBase.h"
#include "R4CharacterRow.generated.h"

class UAnimMontage;
class UUserWidget;
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
	, ChampionSelectedAnim ( nullptr )
	, BaseStatRowPK( DTConst::G_InvalidPK )
	, StatusBarClass( nullptr )
	, StatusBarDrawSize( FVector2d::ZeroVector )
	, StatusBarRelativeLocation( FVector::ZeroVector )
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

	// Champion Select Anim
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Anim" )
	TSoftObjectPtr<UAnimMontage> ChampionSelectedAnim;
	
	// 캐릭터 사망 모션. ( Loop로 설정할 것 )
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Anim" )
	TSoftObjectPtr<UAnimMontage> DeadAnim;
	
	// 해당 캐릭터의 기본 스탯 정보를 담은 Data table의 Pk
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	int32 BaseStatRowPK;

	// 해당 캐릭터가 사용할 Skill Set.
	// 순서대로 Skill Comp에 등록되므로, Player Skill 등록 시 순서에 주의.
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Skill" )
	TArray<TSubclassOf<UR4SkillBase>> Skills;

	// 해당 캐릭터가 사용할 Status Bar Widget Class
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Widget" )
	TSubclassOf<UUserWidget> StatusBarClass;

	// 해당 캐릭터가 사용할 Status Bar의 Draw Size.
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Widget" )
	FVector2D StatusBarDrawSize;

	// 해당 캐릭터가 사용할 Status Bar의 Relative Location.
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Widget" )
	FVector StatusBarRelativeLocation;
};

GENERATE_DT_PTR_H( FR4CharacterRow );