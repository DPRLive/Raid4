#include "R4DACameraInitData.h"
#include "../../Character/CharacterBase.h"

#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/SpringArmComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4DACameraInitData)

UR4DACameraInitData::UR4DACameraInitData()
{
	bUseControllerRotationYaw = false;
	
	bOrientRotationToMovement = false;
	bUseControllerDesiredRotation = false;
	RotationRate = FRotator();

	TargetArmLength = 0.f;
	RelativeRotation = FRotator();
	bUsePawnControlRotation = false;
	bInheritPitch = false;
	bInheritYaw = false;
	bInheritRoll = false;
	bDoCollisionTest = false;
}

/**
 * Character가 Data를 받아갈 수 있도록 한다.
 */
void UR4DACameraInitData::LoadDataToCharacter(ACharacterBase* InCharacter)
{
	if (InCharacter == nullptr)
		return;

	InCharacter->bUseControllerRotationYaw = bUseControllerRotationYaw;

	if (UCharacterMovementComponent* moveComp = InCharacter->GetCharacterMovement())
	{
		moveComp->bOrientRotationToMovement = bOrientRotationToMovement;
		moveComp->bUseControllerDesiredRotation = bUseControllerDesiredRotation;
		moveComp->RotationRate = RotationRate;
	}

	if(USpringArmComponent* springArm = InCharacter->FindComponentByClass<USpringArmComponent>())
	{
		springArm->TargetArmLength = TargetArmLength;
		springArm->SetRelativeRotation(RelativeRotation);
		springArm->bUsePawnControlRotation = bUsePawnControlRotation;
		springArm->bInheritPitch = bInheritPitch;
		springArm->bInheritYaw = bInheritYaw;
		springArm->bInheritRoll = bInheritRoll;
		springArm->bDoCollisionTest = bDoCollisionTest;
	}
}
