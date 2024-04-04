// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerInputComponent.h"
#include "../Interface/R4PlayerSkillInputable.h"
#include "../Interface/R4MouseMovable.h"
#include "../Interface/R4PlayerInputCompInterface.h"

#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <GameFramework/PlayerController.h>
#include <Engine/LocalPlayer.h>
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
	if(IR4PlayerInputCompInterface* owner = Cast<IR4PlayerInputCompInterface>(GetOwner()))
	{
		owner->GetOnSetupPlayerInput().AddUObject(this, &UR4PlayerInputComponent::_InitializePlayerInput);
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

	IR4PlayerInputCompInterface* owner = Cast<IR4PlayerInputCompInterface>(GetOwner());
	if(owner == nullptr)
	{
		LOG_ERROR(R4Input, TEXT("Owner does not implement IR4PlayerInputCompInterface."));
		return;
	}

	const APlayerController* playerController = owner->GetPlayerController();
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
	if(IR4MouseMovable* owner = Cast<IR4MouseMovable>(GetOwner()))
	{
		owner->StopMove();
	}
}

/**
 *  이동 입력 Triggered
 */
void UR4PlayerInputComponent::OnInputMoveTriggered()
{
	CachedTriggerTime += GetWorld()->GetDeltaSeconds();

	IR4MouseMovable* mouseMoveObj = Cast<IR4MouseMovable>(GetOwner());
	if(mouseMoveObj == nullptr)
	{
		LOG_WARN(R4Input, TEXT("Owner is nullptr."));
		return;
	}
	
	APlayerController* playerController = nullptr;
	if(IR4PlayerInputCompInterface* owner = Cast<IR4PlayerInputCompInterface>(GetOwner()))
	{
		playerController = owner->GetPlayerController();
	}

	FHitResult hit;
	if(AActor* ownerActor = GetOwner(); ownerActor != nullptr && playerController != nullptr &&
		playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, hit))
	{
		FVector dir = hit.Location - ownerActor->GetActorLocation();
		dir.Z = 0.f;
		dir.Normalize();
		
		mouseMoveObj->AddMovement(dir);
		CachedLastHitLocation = hit.Location;
	}
}

/**
 *  이동 입력 종료
 */
void UR4PlayerInputComponent::OnInputMoveCompleted()
{
	IR4MouseMovable* mouseMoveObj = Cast<IR4MouseMovable>(GetOwner());
	if(mouseMoveObj == nullptr)
	{
		LOG_WARN(R4Input, TEXT("Owner is nullptr."));
		return;
	}

	// 짧은 입력이면, 위치로 이동 실행
	if(CachedTriggerTime < ShortTriggerThreshold)
	{
		UtilEffect::SpawnNiagaraAtLocation_Local(FXCursor, CachedLastHitLocation, FRotator::ZeroRotator, FVector(1.f), GetWorld());
		mouseMoveObj->MoveToLocation(CachedLastHitLocation);
	}
	else
	{
		// 긴 입력이었다면, 멈춤
		mouseMoveObj->StopMove();
	}
	
	CachedTriggerTime = 0.f;
}

/**
 *  스킬 입력 시작
 */
void UR4PlayerInputComponent::OnInputSkillStarted(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputable* owner = Cast<IR4PlayerSkillInputable>(GetOwner()))
	{
		owner->OnInputSkillStarted(InSkillIndex);
	}
}

/**
 *  스킬 입력 중
 */
void UR4PlayerInputComponent::OnInputSkillTriggered(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputable* owner = Cast<IR4PlayerSkillInputable>(GetOwner()))
	{
		owner->OnInputSkillTriggered(InSkillIndex);
	}
}

/**
 *  스킬 입력 종료
 */
void UR4PlayerInputComponent::OnInputSkillCompleted(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(IR4PlayerSkillInputable* owner = Cast<IR4PlayerSkillInputable>(GetOwner()))
	{
		owner->OnInputSkillCompleted(InSkillIndex);
	}
}
