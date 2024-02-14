// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerInputComponent.h"
#include "../Character/PlayerCharacter.h"
#include "../Interface/R4PlayerSkillInputInterface.h"

#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <GameFramework/PlayerController.h>
#include <Engine/LocalPlayer.h>
#include <Blueprint/AIBlueprintHelperLibrary.h>
#include <GameFramework/CharacterMovementComponent.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4PlayerInputComponent)

UR4PlayerInputComponent::UR4PlayerInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	
	ShortTriggerThreshold = 0.3f;
	CachedTriggerTime = 0.f;
}

/**
 *	컴포넌트 초기화
 */
void UR4PlayerInputComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Input Init을 대기한다.
	if(APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner()))
	{
		player->OnSetupPlayerInput.AddUObject(this, &UR4PlayerInputComponent::_InitializePlayerInput);
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
	// 액션 바인딩
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InPlayerInputComponent))
	{
		EnhancedInputComponent->ClearActionBindings();

		// 이동 입력 액션 바인딩
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &UR4PlayerInputComponent::OnInputMoveStarted);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UR4PlayerInputComponent::OnInputMoveTriggered);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &UR4PlayerInputComponent::OnInputMoveCompleted);

		// 스킬 입력 액션 바인딩
		for( const TPair<ESkillIndex, TObjectPtr<UInputAction>>& skillAction : SkillActions )
		{
			// 스킬 index도 같이 넘긴다.
			EnhancedInputComponent->BindAction(skillAction.Value, ETriggerEvent::Started, this, &UR4PlayerInputComponent::OnInputSkillStarted, skillAction.Key);
			EnhancedInputComponent->BindAction(skillAction.Value, ETriggerEvent::Triggered, this, &UR4PlayerInputComponent::OnInputSkillTriggered, skillAction.Key);
			EnhancedInputComponent->BindAction(skillAction.Value, ETriggerEvent::Completed, this, &UR4PlayerInputComponent::OnInputSkillCompleted, skillAction.Key);
		}
	}

	APawn* owner = Cast<APawn>(GetOwner());
	if(owner == nullptr)
	{
		LOG_WARN(R4Input, TEXT("Owner is nullptr."));
		return;
	}

	APlayerController* playerController = Cast<APlayerController>(owner->GetController());
	if(playerController == nullptr)
	{
		LOG_WARN(R4Input, TEXT("Owner's Controller is nullptr."));
		return;
	}
	
	// IMC 바인딩
	if(UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
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
 *  이동 입력 시작
 */
void UR4PlayerInputComponent::OnInputMoveStarted()
{
	if(ACharacter* owner = Cast<ACharacter>(GetOwner()))
	{
		owner->GetMovementComponent()->StopMovementImmediately();
	}
}

/**
 *  이동 입력 Triggered
 */
void UR4PlayerInputComponent::OnInputMoveTriggered()
{
	APawn* owner = Cast<APawn>(GetOwner());
	if(owner == nullptr)
	{
		LOG_WARN(R4Input, TEXT("Owner is nullptr."));
		return;
	}
	
	APlayerController* playerController = Cast<APlayerController>(owner->GetController());
	if(playerController == nullptr)
	{
		LOG_WARN(R4Input, TEXT("Owner's Controller is nullptr."));
		return;
	}

	CachedTriggerTime += GetWorld()->GetDeltaSeconds();
	
	FHitResult hit;
	if(playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, hit))
	{
		FVector dir = hit.Location - owner->GetActorLocation();
		dir.Z = 0.f;
		dir.Normalize();
		
		owner->AddMovementInput(dir);
		CachedLastHitLocation = hit.Location;
	}
}

/**
 *  이동 입력 종료
 */
void UR4PlayerInputComponent::OnInputMoveCompleted()
{
	ACharacter* owner = Cast<ACharacter>(GetOwner());
	if(owner == nullptr)
	{
		LOG_WARN(R4Input, TEXT("Owner is nullptr."));
		return;
	}

	APlayerController* playerController = Cast<APlayerController>(owner->GetController());
	if(playerController == nullptr)
	{
		LOG_WARN(R4Input, TEXT("Owner's Controller is nullptr."));
		return;
	}

	// 짧은 입력이면, 위치로 이동 실행
	if(CachedTriggerTime < ShortTriggerThreshold)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(playerController, CachedLastHitLocation);
		UtilEffect::SpawnNiagaraAtLocation_Local(FXCursor, CachedLastHitLocation, FRotator::ZeroRotator, FVector(1.f), GetWorld());
	}
	else
	{
		// 긴 입력이었다면, 멈춤
		owner->GetMovementComponent()->StopMovementImmediately();
	}
	
	CachedTriggerTime = 0.f;
}

/**
 *  스킬 입력 시작
 */
void UR4PlayerInputComponent::OnInputSkillStarted(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputInterface* owner = Cast<IR4PlayerSkillInputInterface>(GetOwner()))
	{
		owner->OnInputSkillStarted(InSkillIndex);
	}
}

/**
 *  스킬 입력 중
 */
void UR4PlayerInputComponent::OnInputSkillTriggered(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputInterface* owner = Cast<IR4PlayerSkillInputInterface>(GetOwner()))
	{
		owner->OnInputSkillTriggered(InSkillIndex);
	}
}

/**
 *  스킬 입력 종료
 */
void UR4PlayerInputComponent::OnInputSkillCompleted(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputInterface* owner = Cast<IR4PlayerSkillInputInterface>(GetOwner()))
	{
		owner->OnInputSkillCompleted(InSkillIndex);
	}
}
