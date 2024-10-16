// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerCharacter.h"
#include "../../Input/R4PlayerInputComponent.h"
#include "../../Camera/R4CameraManageComponent.h"
#include "../../Skill/Player/R4PlayerSkillComponent.h"
#include "../../Data/Character/R4DataAsset_PCCommonData.h"
#include "../../PlayerState/R4PlayerStateInterface.h"
#include "../../UI/HUD/R4InGameHUDWidget.h"

#include <Components/CapsuleComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/PlayerController.h>
#include <GameFramework/PlayerState.h>


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

	// Set Profile Player
	if ( GetCapsuleComponent() )
		GetCapsuleComponent()->SetCollisionProfileName( Collision::G_ProfilePlayer );
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

	// Init CameraManageComp
	CameraManageComp->SetCameraComp( CameraComp );
	CameraManageComp->SetSpringArmComp( SpringArmComp );
}

void AR4PlayerCharacter::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	OnSetupPlayerInputDelegate.Clear();
	Super::EndPlay( EndPlayReason );
}

/**
 *  Player State로부터 Character DT Key 값 Load (Server)
 */
void AR4PlayerCharacter::PossessedBy( AController* InNewController )
{
	Super::PossessedBy( InNewController );

	if ( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( GetPlayerState() ) )
		PushDTData( playerState->GetCharacterId() );
}

/**
 *  Player State로부터 Character DT Key 값 Load (Client)
 */
void AR4PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if ( IR4PlayerStateInterface* playerState = Cast<IR4PlayerStateInterface>( GetPlayerState() ) )
		PushDTData( playerState->GetCharacterId() );
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
 *  Skill Widget에 데이터 제공
 */
void AR4PlayerCharacter::SetupSkillWidget( UUserWidget* InWidget )
{
	if ( !IsValid( SkillComp ) )
		return;
	
	// Bind Skill Widget
	if ( UR4InGameHUDWidget* hudWidget = Cast<UR4InGameHUDWidget>( InWidget ) )
	{
		hudWidget->OnUpdateSkillCooldown();
		
		SkillComp->OnSkillCooldownDelegate.AddWeakLambda( hudWidget, [hudWidget]( int32 InSkillIndex )
		{
			hudWidget->OnUpdateSkillCooldown();
		} );
	}
}

/**
 *  Cooldown 반환
 *  @return : 해당 Skill index에 해당하는 남은 쿨타임, 유효하지 않을 시 -1 반환.
 */
float AR4PlayerCharacter::GetSkillCooldownRemaining( int32 InSkillIndex )
{
	if( IsValid( SkillComp ) )
		return SkillComp->GetSkillCooldownRemaining( InSkillIndex );

	return -1.f;
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
 *  캐릭터 죽음 처리
 */
void AR4PlayerCharacter::Dead()
{
	Super::Dead();

	CameraManageComp->ClearResizeSpringArm();

	// Disable input.
	DisableInput( GetController<APlayerController>() );
}

/**
 *  PlayerCharacter들의 공통된 데이터를 초기화
 */
void AR4PlayerCharacter::_InitPlayerCharacterCommonData()
{
	if ( !ensureMsgf( IsValid(PlayerCharacterCommonData), TEXT("PlayerCharacterCommonData is nullptr.") ) )
		return;

	bUseControllerRotationYaw = PlayerCharacterCommonData->bUseControllerRotationYaw;

	if ( UCharacterMovementComponent* moveComp = GetCharacterMovement() )
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
