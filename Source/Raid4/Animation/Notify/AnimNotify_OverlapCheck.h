// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Animation/AnimNotifies/AnimNotify.h>
#include "AnimNotify_OverlapCheck.generated.h"

/**
 * Overlap을 체크하는 Notify
 */
UCLASS()
class RAID4_API UAnimNotify_OverlapCheck : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_OverlapCheck();
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	// Overlap 결과를 처리
	void _ProcessOverlapActor(const AActor* InActor) const;
private:
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
