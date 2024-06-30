// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../Interface/R4Detectable.h"
#include <Animation/AnimNotifies/AnimNotify.h>
#include "AnimNotify_OverlapCheck.generated.h"

class UNiagaraSystem;

/**
 * Overlap 시 출력할 Effect 정보
 * TODO : 오버랩 이펙트 관련 헤더따로 만들기, util effect로 빼보기, controller에 RPC들 모은 component 만들기
 */
USTRUCT()
struct FOverlapEffectInfo
{
	GENERATED_BODY()

	FOverlapEffectInfo() : EffectType(EOverlapEffectType::ClosestPoint) {}

	// 출력할 Particle
	UPROPERTY( EditAnywhere )
	TSoftObjectPtr<UNiagaraSystem> Particle;

	// 출력할 타입
	UPROPERTY( EditAnywhere )
	EOverlapEffectType EffectType;
};

/**
 * Overlap을 체크하는 Notify
 */
UCLASS()
class RAID4_API UAnimNotify_OverlapCheck : public UAnimNotify, public IR4Detectable
{
	GENERATED_BODY()

public:
	UAnimNotify_OverlapCheck();
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	// ~ Begin IR4Detectable
	virtual FOnDetectDelegate& OnBeginDetect() override { return OnBeginDetectDelegate; }
	virtual FOnDetectDelegate& OnEndDetect() override { return OnEndDetectDelegate; }
	// ~ End IR4Detectable
	
private:
	// Overlap 결과를 처리
	void _ProcessOverlapActor(const AActor* InActor) const;

	// Effect를 출력
	void _SpawnNiagara(const AActor* InInstigator, const FOverlapResult& InResult) const;
private:
	// Detect시 broadcast
	FOnDetectDelegate OnBeginDetectDelegate;

	// Detect 종료 시 broadcast
	FOnDetectDelegate OnEndDetectDelegate;
	
	// Overlap할 형태 결정
	UPROPERTY( EditAnywhere, Category="AnimNotify", meta =(AllowPrivateAccess = true) )
	EOverlapShape Shape;
	
	/**
	 *	Overlap 체크 시 사용할 Shape의 모양을 결정하는 변수 (X, Y, Z 차례대로 들어감)
	 *	Box : BoxHalfExtent (XYZ)
	 *	Sphere : Radius
	 *	Capsule : Radius, HalfHeight
	 *	Circle Arc : Radius, HalfHeight, Angle (0 ~ 360)
	 */
	UPROPERTY( EditAnywhere, Category="AnimNotify", meta =(AllowPrivateAccess = true) )
	FVector ShapeParam;
	
	// overlap을 체크할 Mesh로부터의 (일반적으로 캐릭터 양발 중심 정도) 상대 위치.
	UPROPERTY( EditAnywhere, Category="AnimNotify", meta =(AllowPrivateAccess = true) )
	FVector RelativeLoc;
	
	/**
	 *	Overlap시 상대적인 Rotation.
	 *	X : Roll, Y : Pitch, Z : Yaw
	 */
	UPROPERTY( EditAnywhere, Category="AnimNotify", meta =(AllowPrivateAccess = true) )
	FRotator RelativeRot;
	
	// Overlap 체크 시 사용할 Collision Profile Name
	UPROPERTY( EditAnywhere, Category="AnimNotify", meta =(AllowPrivateAccess = true) )
	FName Profile;

	// Overlap 시 출력할 파티클들
	UPROPERTY( EditAnywhere, Category="AnimNotify", meta =(AllowPrivateAccess = true) )
	TArray<FOverlapEffectInfo> Particles;
	
	// Debug 할 것인지 설정
	UPROPERTY( EditAnywhere, Category="Debug", meta =(AllowPrivateAccess = true) )
	uint8 bDrawDebug:1;

	// Debug를 얼마나 그릴것인지 설정
	UPROPERTY( EditAnywhere, Category="Debug", meta =(AllowPrivateAccess = true) )
	float DebugTime;
	
	// Debug Color 설정
	UPROPERTY( EditAnywhere, Category="Debug", meta =(AllowPrivateAccess = true) )
	FColor DebugColor;
};
