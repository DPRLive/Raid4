// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerInputComponent.h"
#include "R4PlayerInputInterface.h"

#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <GameFramework/PlayerController.h>
#include <Engine/LocalPlayer.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerInputComponent)

UR4PlayerInputComponent::UR4PlayerInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

/**
 *	컴포넌트 초기화
 */
void UR4PlayerInputComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Owner 등록
	if(IsValid(GetOwner()))
	{
		if(!ensureMsgf(GetOwner()->GetClass()->ImplementsInterface(UR4PlayerInputInterface::StaticClass()),
			TEXT("UR4PlayerInputComponent's owner must implement IR4PlayerInputInterface.")))
				return;
		
		Owner = Cast<IR4PlayerInputInterface>(GetOwner());
	}

	// Init
	if(Owner.IsValid())
	{
		Owner->OnSetupPlayerInput().AddUObject(this, &UR4PlayerInputComponent::_InitializePlayerInput);
	}
}

/**
 *  begin play
 */
void UR4PlayerInputComponent::BeginPlay()
{
	Super::BeginPlay();
}

/**
 *  입력을 바인딩 하는 함수
 */
void UR4PlayerInputComponent::_InitializePlayerInput(UInputComponent* InPlayerInputComponent)
{
	if(!Owner.IsValid())
		return;
	
	// 액션 바인딩
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InPlayerInputComponent); IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->ClearActionBindings();

		// 이동 입력 액션 바인딩
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UR4PlayerInputComponent::_OnInputMoveTriggered);

		// Look 입력 액션 바인딩
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &UR4PlayerInputComponent::_OnInputLookTriggered);

		// 점프 입력 액션 바인딩
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &UR4PlayerInputComponent::_OnInputJumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &UR4PlayerInputComponent::_OnInputJumpCompleted);
		
		// 회피 입력 액션 바인딩
		EnhancedInputComponent->BindAction(EvasionAction, ETriggerEvent::Started, this, &UR4PlayerInputComponent::_OnInputEvasionStarted);
		
		// 스킬 입력 액션 바인딩
		for( const TPair<ESkillIndex, TObjectPtr<UInputAction>>& skillAction : SkillActions )
		{
			// 스킬 index도 같이 넘긴다.
			EnhancedInputComponent->BindAction(skillAction.Value, ETriggerEvent::Started, this, &UR4PlayerInputComponent::_OnInputSkillStarted, skillAction.Key);
			EnhancedInputComponent->BindAction(skillAction.Value, ETriggerEvent::Triggered, this, &UR4PlayerInputComponent::_OnInputSkillTriggered, skillAction.Key);
			EnhancedInputComponent->BindAction(skillAction.Value, ETriggerEvent::Completed, this, &UR4PlayerInputComponent::_OnInputSkillCompleted, skillAction.Key);
		}
	}

	const APlayerController* playerController = Owner->GetPlayerController();
	if(!IsValid(playerController))
	{
		LOG_WARN(R4Input, TEXT("Owner's Controller is nullptr."));
		return;
	}
	
	// IMC 바인딩
	if(UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()); IsValid(subsystem))
	{
		if(subsystem->HasMappingContext(InputMapping))
		{
			LOG_WARN(R4Input, TEXT("Owner already has this mapping context."));
			return;
		}

		subsystem->AddMappingContext(InputMapping, 0);
	}
}

/**
 *  이동 입력
 */
void UR4PlayerInputComponent::_OnInputMoveTriggered(const FInputActionValue& Value)
{
	if(Owner.IsValid())
	{
		// 2D로 분해
		FVector2D movementVector = Value.Get<FVector2D>();
		
		Owner->OnInputMoveTriggered(movementVector);
	}
}

/**
 *  마우스 Look 입력
 */
void UR4PlayerInputComponent::_OnInputLookTriggered(const FInputActionValue& Value)
{
	if(Owner.IsValid())
	{
		// Rotator로 분해
		FVector2D lookVector = Value.Get<FVector2D>();

		Owner->OnInputLookTriggered(FRotator(lookVector.Y, lookVector.X, 0.f));
	}
}

/**
 *  점프 입력 시작
 */
void UR4PlayerInputComponent::_OnInputJumpStarted(const FInputActionValue& Value)
{
	if(Owner.IsValid())
		Owner->OnInputJumpStarted();
}

/**
 *  점프 입력 종료
 */
void UR4PlayerInputComponent::_OnInputJumpCompleted(const FInputActionValue& Value)
{
	if(Owner.IsValid())
		Owner->OnInputJumpCompleted();
}

/**
 *  회피 입력 종료
 */
void UR4PlayerInputComponent::_OnInputEvasionStarted(const FInputActionValue& Value)
{
	if(Owner.IsValid())
		Owner->OnInputEvasionStarted();
}

/**
 *  스킬 입력 시작
 */
void UR4PlayerInputComponent::_OnInputSkillStarted(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(Owner.IsValid())
		Owner->OnInputSkillStarted(InSkillIndex);
}

/**
 *  스킬 입력 중
 */ 
void UR4PlayerInputComponent::_OnInputSkillTriggered(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(Owner.IsValid())
		Owner->OnInputSkillTriggered(InSkillIndex);
}

/**
 *  스킬 입력 종료
 */
void UR4PlayerInputComponent::_OnInputSkillCompleted(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(Owner.IsValid())
		Owner->OnInputSkillCompleted(InSkillIndex);
}
