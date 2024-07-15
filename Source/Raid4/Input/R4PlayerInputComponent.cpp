// Fill out your copyright notice in the Description page of Project Settings.


#include "R4PlayerInputComponent.h"
#include "../Skill/Player/R4PlayerSkillInterface.h"
#include "../Movement/R4MouseMovable.h"
#include "R4PlayerInputCompInterface.h"

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

	if(!IsValid(GetOwner()))
		return;
		
	// Input Init을 대기한다.
	if(IR4PlayerInputCompInterface* owner = Cast<IR4PlayerInputCompInterface>(GetOwner()))
	{
		owner->OnSetupPlayerInput().AddUObject(this, &UR4PlayerInputComponent::_InitializePlayerInput);
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
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InPlayerInputComponent); IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->ClearActionBindings();

		// 이동 입력 액션 바인딩
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &UR4PlayerInputComponent::_OnInputMoveStarted);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UR4PlayerInputComponent::_OnInputMoveTriggered);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &UR4PlayerInputComponent::_OnInputMoveCompleted);

		// 스킬 입력 액션 바인딩
		for( const TPair<ESkillIndex, TObjectPtr<UInputAction>>& skillAction : SkillActions )
		{
			// 스킬 index도 같이 넘긴다.
			EnhancedInputComponent->BindAction(skillAction.Value, ETriggerEvent::Started, this, &UR4PlayerInputComponent::_OnInputSkillStarted, skillAction.Key);
			EnhancedInputComponent->BindAction(skillAction.Value, ETriggerEvent::Triggered, this, &UR4PlayerInputComponent::_OnInputSkillTriggered, skillAction.Key);
			EnhancedInputComponent->BindAction(skillAction.Value, ETriggerEvent::Completed, this, &UR4PlayerInputComponent::_OnInputSkillCompleted, skillAction.Key);
		}
	}

	if(!IsValid(GetOwner()))
		return;
	
	IR4PlayerInputCompInterface* owner = Cast<IR4PlayerInputCompInterface>(GetOwner());
	if(owner == nullptr)
	{
		LOG_ERROR(R4Input, TEXT("Owner does not implement IR4PlayerInputCompInterface."));
		return;
	}

	const APlayerController* playerController = owner->GetPlayerController();
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
 *  이동 입력 시작
 */
void UR4PlayerInputComponent::_OnInputMoveStarted()
{
	if(!IsValid(GetOwner()))
		return;
	
	if(IR4MouseMovable* owner = Cast<IR4MouseMovable>(GetOwner()))
	{
		owner->StopMove();
	}
}

/**
 *  이동 입력 Triggered
 */
void UR4PlayerInputComponent::_OnInputMoveTriggered()
{
	if(!IsValid(GetOwner()))
		return;
	
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
	if(AActor* ownerActor = GetOwner();
		IsValid(ownerActor)
		&& IsValid(playerController)
		&& playerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, hit))
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
void UR4PlayerInputComponent::_OnInputMoveCompleted()
{
	if(!IsValid(GetOwner()))
		return;
	
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
void UR4PlayerInputComponent::_OnInputSkillStarted(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(!IsValid(GetOwner()))
		return;
	
	if(IR4PlayerSkillInterface* owner = Cast<IR4PlayerSkillInterface>(GetOwner()))
	{
		owner->OnInputSkillStarted(InSkillIndex);
	}
}

/**
 *  스킬 입력 중
 */
void UR4PlayerInputComponent::_OnInputSkillTriggered(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(!IsValid(GetOwner()))
		return;
	
	if(IR4PlayerSkillInterface* owner = Cast<IR4PlayerSkillInterface>(GetOwner()))
	{
		owner->OnInputSkillTriggered(InSkillIndex);
	}
}

/**
 *  스킬 입력 종료
 */
void UR4PlayerInputComponent::_OnInputSkillCompleted(const FInputActionValue& InValue, ESkillIndex InSkillIndex)
{
	if(!IsValid(GetOwner()))
		return;
	
	if(IR4PlayerSkillInterface* owner = Cast<IR4PlayerSkillInterface>(GetOwner()))
	{
		owner->OnInputSkillCompleted(InSkillIndex);
	}
}
