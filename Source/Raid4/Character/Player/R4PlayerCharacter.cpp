// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerCharacter.h"
#include "../../Input/R4PlayerInputComponent.h"
#include "../../Camera/R4CameraManageComponent.h"
#include "../../Skill/Player/R4PlayerSkillComponent.h"
#include "../../Data/DataAsset/R4DataAsset_PCCommonData.h"

#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerCharacter)

/**
 *  생성자. Skill Comp를 Player용으로 변경. 찾아보니 SetDefaultSubobjectClass를 여러번 .으로 이어서 쓸 수 있다
 */
AR4PlayerCharacter::AR4PlayerCharacter(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer.SetDefaultSubobjectClass<UR4PlayerSkillComponent>(FName(TEXT("SkillComp"))))
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationYaw = false;
	
	// Input Component
	PlayerInputComp = CreateDefaultSubobject<UR4PlayerInputComponent>(TEXT("PlayerInputComp"));

	// SpringArm Comp
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
		
	// Camera Component
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	// Camera Manage Comp
	CameraManageComp = CreateDefaultSubobject<UR4CameraManageComponent>(TEXT("CameraManageComp"));
}

/**
 *  Post Init
 */
void AR4PlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	_InitPlayerCharacterCommonData();
}

/**
 *  begin play
 */
void AR4PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AR4PlayerCharacter::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	OnSetupPlayerInputDelegate.Clear();
	Super::EndPlay( EndPlayReason );
}

/**
 *  SetupPlayerInputComponent, Player Controller가 Pose 할 때 호출
 */
void AR4PlayerCharacter::SetupPlayerInputComponent(UInputComponent* InPlayerInputComponent)
{
	Super::SetupPlayerInputComponent(InPlayerInputComponent);

	if(OnSetupPlayerInputDelegate.IsBound())
		OnSetupPlayerInputDelegate.Broadcast(InPlayerInputComponent);
	
}

/**
 *  PlayerController를 리턴
 */
APlayerController* AR4PlayerCharacter::GetPlayerController()
{
	return Cast<APlayerController>(GetController());
}

/**
 *  이동 입력 처리
 */
void AR4PlayerCharacter::OnInputMoveTriggered(const FVector2D& InDirection)
{
	if (IsValid(Controller))
	{
		const FRotator yawRotation(0, Controller->GetControlRotation().Yaw, 0);

		const FVector forwardDir = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
		const FVector rightDir = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(forwardDir, InDirection.Y);
		AddMovementInput(rightDir, InDirection.X);
	}
}

/**
 *  마우스 Look 입력 처리
 */
void AR4PlayerCharacter::OnInputLookTriggered(const FRotator& InDelta)
{
	if (IsValid(Controller))
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(InDelta.Yaw);
		AddControllerPitchInput(InDelta.Pitch);
	}
}

/**
 *  점프 입력 시작 처리
 */
void AR4PlayerCharacter::OnInputJumpStarted()
{
	Jump();
}

/**
 *  점프 입력 종료
 */
void AR4PlayerCharacter::OnInputJumpCompleted()
{
	StopJumping();
}


/**
 *  스킬 입력 시작 처리
 */
void AR4PlayerCharacter::OnInputSkillStarted( EPlayerSkillIndex InSkillIndex )
{
	if ( UR4PlayerSkillComponent* skillComp = Cast<UR4PlayerSkillComponent>( SkillComp ) )
		skillComp->OnInputSkillStarted( InSkillIndex );
}

/**
 *  스킬 입력 처리
 */
void AR4PlayerCharacter::OnInputSkillTriggered( EPlayerSkillIndex InSkillIndex )
{
	if ( UR4PlayerSkillComponent* skillComp = Cast<UR4PlayerSkillComponent>( SkillComp ) )
		skillComp->OnInputSkillTriggered( InSkillIndex );
}

/**
 *  스킬 입력 종료 처리
 */
void AR4PlayerCharacter::OnInputSkillCompleted( EPlayerSkillIndex InSkillIndex )
{
	if ( UR4PlayerSkillComponent* skillComp = Cast<UR4PlayerSkillComponent>( SkillComp ) )
		skillComp->OnInputSkillCompleted( InSkillIndex );
}

/**
 *  PlayerCharacter들의 공통된 데이터를 초기화한다.  
 */
void AR4PlayerCharacter::_InitPlayerCharacterCommonData()
{
	if(!ensureMsgf(IsValid(PlayerCharacterCommonData), TEXT("PlayerCharacterCommonData is nullptr.")))
		return;

	bUseControllerRotationYaw = PlayerCharacterCommonData->bUseControllerRotationYaw;
	
	if (UCharacterMovementComponent* moveComp = GetCharacterMovement(); IsValid(moveComp))
	{
		moveComp->bOrientRotationToMovement = PlayerCharacterCommonData->bOrientRotationToMovement;
		moveComp->bUseControllerDesiredRotation = PlayerCharacterCommonData->bUseControllerDesiredRotation;
		moveComp->RotationRate = PlayerCharacterCommonData->RotationRate;
	}

	SpringArmComp->TargetArmLength = PlayerCharacterCommonData->TargetArmLength;
	SpringArmComp->SetRelativeLocation(PlayerCharacterCommonData->RelativeLocation);
	SpringArmComp->SetRelativeRotation(PlayerCharacterCommonData->RelativeRotation);
	SpringArmComp->bUsePawnControlRotation = PlayerCharacterCommonData->bUsePawnControlRotation;
	SpringArmComp->bInheritPitch = PlayerCharacterCommonData->bInheritPitch;
	SpringArmComp->bInheritYaw = PlayerCharacterCommonData->bInheritYaw;
	SpringArmComp->bInheritRoll = PlayerCharacterCommonData->bInheritRoll;
	SpringArmComp->bDoCollisionTest = PlayerCharacterCommonData->bDoCollisionTest;
}
