// Fill out your copyright notice in the Description page of Project Settings.


#include "R4Skill_PlayerPoint.h"

#include <Net/UnrealNetwork.h>
#include <Components/DecalComponent.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Pawn.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4Skill_PlayerPoint)

UR4Skill_PlayerPoint::UR4Skill_PlayerPoint()
{
	CachedNowPointing = false;
}

void UR4Skill_PlayerPoint::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Anim Key를 Replicate 받을 수 있도록 설정
	DOREPLIFETIME_CONDITION( UR4Skill_PlayerPoint, PointingSkillAnimInfo, COND_InitialOnly );
	DOREPLIFETIME_CONDITION( UR4Skill_PlayerPoint, AttackSkillAnimInfo, COND_InitialOnly );

	DOREPLIFETIME( UR4Skill_PlayerPoint, CachedTargetWorldPoint_Server );
}

void UR4Skill_PlayerPoint::BeginPlay()
{
	Super::BeginPlay();

	// Trace에 필요한 정보 캐싱
	if ( APawn* owner = Cast< APawn >( GetOwner() ) )
		CachedPlayerController = owner->GetController<APlayerController>();
}

void UR4Skill_PlayerPoint::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// 지점 선택 중일 시, 지점을 선택
	if( CachedNowPointing )
		_TracePointing();
}

/**
 *  스킬 입력 시작, 첫번 째 입력 시 지점 선택, 두번 째 입력 시 스킬 발동.
 */
void UR4Skill_PlayerPoint::OnInputStarted()
{
	// 스킬이 사용 가능 하고, 지점 선택 중이 아닐 시
	if ( CanActivateSkill() && !CachedNowPointing )
	{
		// 지점 선택 로직 실행
		CachedNowPointing = true;

		// Decal 생성
		_SetupPointingDecal();
		
		// 지점 선택 Anim이 있다면 실행
		if( PointingSkillAnimInfo.SkillAnim != nullptr )
			PlaySkillAnim( PointingSkillAnimInfo );
		
		// Tick on
		SetComponentTickEnabled( true );
		return;
	}

	// 지점 선택 중 재입력 시
	if( CachedNowPointing )
	{
		CachedNowPointing = false;

		// Decal 정리
		_TearDownPointingDecal();
		
		// 서버로 선택한 위치 전송
		_ServerRPC_SendTargetPoint( CachedTargetWorldPoint_Client );
	}
}

/**
 *  특정 Origin 값을 제공.
 *  Server에서 확정한 Owner의 Transform을 기준으로, Server에서 확정한 위치를 설정하여 제공
 *  @param InRequestObj : 계산을 요청한 Object
 *  @param InActor : 계산의 기준이 되는 Actor.
 */
FTransform UR4Skill_PlayerPoint::GetOrigin( const UObject* InRequestObj, const AActor* InActor ) const
{
	FTransform retTrans;

	if ( IsValid( GetOwner() ) )
	{
		retTrans = GetOwner()->GetTransform();
		retTrans.SetLocation( CachedTargetWorldPoint_Server );
	}

	return retTrans;
}

/**
 * Anim을 Play시작 시 호출
 * @param InSkillAnimInfo : Play된 Skill Anim 정보
 * @param InStartServerTime : Skill Anim이 시작된 Server Time
 */
void UR4Skill_PlayerPoint::OnBeginSkillAnim( const FR4SkillAnimInfo& InSkillAnimInfo, float InStartServerTime )
{
	Super::OnBeginSkillAnim( InSkillAnimInfo, InStartServerTime );

	// server & autonomous only
	if( GetOwnerRole() == ROLE_SimulatedProxy )
		return;
	
	// Player Point Skill의 경우 Attack Anim Play 시점을 Skill 사용으로 판정
	// Anim Play 성공 = 스킬 사용으로 판단 및 쿨타임 적용
	if ( InSkillAnimInfo.SkillAnimServerKey == AttackSkillAnimInfo.SkillAnimServerKey )
		SetSkillCoolDownTime( GetSkillCoolDownTime( false ) );
}

/**
 *  현재 Tick이 필요한 상태인지 return.
 *  기본적으로 tick은 off 상태를 유지하며, 특정 함수에서 tick을 on 시키면
 *  해당 함수 false 시 Tick 함수에서 다시 Off.
 */
bool UR4Skill_PlayerPoint::IsNeedTick() const
{
	// Pointing 중이면 Tick을 유지
	return ( Super::IsNeedTick() || CachedNowPointing );
}

/**
 *  지점 선택 Trace
 */
void UR4Skill_PlayerPoint::_TracePointing()
{
	// world or owner가 invalid 하면 진행하지 않음.
	if ( !IsValid( GetWorld() ) || !IsValid( GetOwner() ) )
		return;
	
	FVector ownerLoc = GetOwner()->GetActorLocation();

	// Range AOE Decal 이동
	if ( IsValid( AOEInfo.RangeAOEInstance ) )
		AOEInfo.RangeAOEInstance->SetWorldLocation( ownerLoc );
	
	// Trace에 필요한 정보가 없다면 다시 Caching
	if ( !CachedPlayerController.IsValid() )
	{
		if ( APawn* owner = Cast<APawn>( GetOwner() ) )
			CachedPlayerController = owner->GetController<APlayerController>();
	}

	if ( !CachedPlayerController.IsValid() )
	{
		LOG_WARN( R4Skill, TEXT("Owner Player Controller is invalid.") );
		_SetTargetPointAndDecal( ownerLoc );
		return;
	}
	
	// 화면 크기 가져오기
	int32 viewportXSize, viewportYSize;
	CachedPlayerController->GetViewportSize( viewportXSize, viewportYSize );

	// 화면 중앙 좌표
	FVector2D screenCenter( viewportXSize * 0.5f, viewportYSize * 0.5f );
	
	// screenCenter -> world
	FVector worldLoc, worldDir;
	if ( CachedPlayerController->DeprojectScreenPositionToWorld( screenCenter.X, screenCenter.Y, worldLoc, worldDir ) )
	{
		// Trace 시작과 끝 설정
		FVector start = worldLoc;
		FVector end = start + ( worldDir * Skill::G_PointSkillTraceLength );

		FHitResult hitResult;
		FCollisionQueryParams params;

		// Line Trace
		bool bHit = GetWorld()->LineTraceSingleByChannel( hitResult, start, end, SKILL_POINT_TRACE_CHANNEL, params );

		// Range Limit Check, 넘어가면 보정
		if( !bHit
			|| ( FVector::DistSquared( hitResult.Location, ownerLoc ) > ( AOEInfo.RangeRadius * AOEInfo.RangeRadius ) ) )
		{
			FVector traceEnd = bHit ? hitResult.Location : end;
			FVector ownerToTraceEndDir = ( traceEnd - ownerLoc ).GetSafeNormal();
			
			// Actor의 중점에서 trace end 방향으로, 가장 Range Limit안 가장 먼 지점으로 이동
			hitResult.Location = ownerLoc + ownerToTraceEndDir * AOEInfo.RangeRadius;
		}

		// Trace에 맞은 지점에 Decal 이동 및 지점 Caching
		_SetTargetPointAndDecal( hitResult.Location );
	}
	else
		_SetTargetPointAndDecal( ownerLoc );
}

/**
 *  Decal 생성
 */
void UR4Skill_PlayerPoint::_SetupPointingDecal()
{
	if ( IsValid( GetOwner() ) )
	{
		// TODO : Decal Response 조정
		// Range AOE 생성
		float rangeAOESize = AOEInfo.RangeAOE.GetDecalSizeByActualRadius( AOEInfo.RangeRadius );
		AOEInfo.RangeAOEInstance = UGameplayStatics::SpawnDecalAtLocation( GetWorld(), AOEInfo.RangeAOE.AOEDecal, FVector( rangeAOESize ), GetOwner()->GetActorLocation() );

		// Point AOE 생성
		float pointAOESize = AOEInfo.PointAOE.GetDecalSizeByActualRadius( AOEInfo.PointRadius );
		AOEInfo.PointAOEInstance = UGameplayStatics::SpawnDecalAtLocation( GetWorld(), AOEInfo.PointAOE.AOEDecal, FVector( pointAOESize ), GetOwner()->GetActorLocation() );
	}
}

/**
 *  Decal 정리
 */
void UR4Skill_PlayerPoint::_TearDownPointingDecal() const
{
	if ( IsValid( AOEInfo.RangeAOEInstance ) )
		AOEInfo.RangeAOEInstance->DestroyComponent();
	
	if ( IsValid( AOEInfo.PointAOEInstance ) )
		AOEInfo.PointAOEInstance->DestroyComponent();
}

/**
 *  지점 지정, Decal도 유효하다면 같이 이동.
 */
void UR4Skill_PlayerPoint::_SetTargetPointAndDecal( const FVector& InWorldLoc )
{
	CachedTargetWorldPoint_Client = InWorldLoc;
	
	// Point AOE Decal 이동
	if ( IsValid( AOEInfo.PointAOEInstance ) )
		AOEInfo.PointAOEInstance->SetWorldLocation( InWorldLoc );
}

/**
 *  Server로 선택한 지점 전송
 */
void UR4Skill_PlayerPoint::_ServerRPC_SendTargetPoint_Implementation( const FVector_NetQuantize& InWorldLoc )
{
	// Skill을 사용할 수 없는 상태에서 들어오는 위치는 무시
	if( !CanActivateSkill() )
		return;

	// 모든 Client들에게 지점 Replicate.
	CachedTargetWorldPoint_Server = InWorldLoc;
	_OnRep_TargetWorldPoint();
}

/**
 *  validation check, 허용 범위 내에 있는지 판별
 */
bool UR4Skill_PlayerPoint::_ServerRPC_SendTargetPoint_Validate( const FVector_NetQuantize& InWorldLoc )
{
	if ( !GetOwner() )
		return false;
	
	float acceptLength = AOEInfo.RangeRadius + Validation::G_AcceptMinLength;
	return ( FVector::DistSquared( GetOwner()->GetActorLocation(), InWorldLoc ) < ( acceptLength * acceptLength ) );
}

/**
 *  Target Point가 Server로 인해 확정 시 호출
 */
void UR4Skill_PlayerPoint::_OnRep_TargetWorldPoint()
{
	// Locally control 상태가 받았으면, 공격 anim play
	if( CachedPlayerController.IsValid() && CachedPlayerController->IsLocalPlayerController() )
		PlaySkillAnim( AttackSkillAnimInfo );
}