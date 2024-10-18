#pragma once

#include "../R4RowBase.h"
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
	: CharacterName ( FString() )
	, CapsuleHalfHeight( 88.f )
	, CapsuleRadius ( 34.f )
	, AnimInstance( nullptr )
	, StatusBarClass( nullptr )
	, StatusBarDrawSize( FVector2d::ZeroVector )
	, StatusBarRelativeLocation( FVector::ZeroVector )
	, ResourceRowPK( DTConst::G_InvalidPK )
	, BaseStatRowPK( DTConst::G_InvalidPK )
	{}

	// Character Name
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	FString CharacterName;
		
	// Capsule Half Height
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Collision" )
	float CapsuleHalfHeight;
	
	// Capsule Size
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Collision" )
	float CapsuleRadius;
	
	// 캐릭터에 사용할 Anim Instance
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Anim" )
	TSubclassOf<UAnimInstance> AnimInstance;

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

	// 해당 캐릭터의 Resource를 담은 Datatable의 Pk
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Resource" )
	int32 ResourceRowPK;
	
	// 해당 캐릭터의 기본 스탯 정보를 담은 Data table의 Pk
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = "Stat" )
	int32 BaseStatRowPK;
};

GENERATE_DT_PTR_H( FR4CharacterRow );