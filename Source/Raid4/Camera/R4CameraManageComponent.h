
#pragma once

#include <Components/ActorComponent.h>

#include "R4CameraManageComponent.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 *  Player의 Camera를 관리해주는 컴포넌트
 */
UCLASS( ClassGroup=(Camera), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4CameraManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4CameraManageComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent( float DeltaTime, enum ELevelTick TickType,
	                            FActorComponentTickFunction* ThisTickFunction ) override;

public:
	// Setter
	void SetCameraComp( UCameraComponent* InCameraComp );
	void SetSpringArmComp( USpringArmComponent* InSpringArmComp );

	// Getter
	FORCEINLINE UCameraComponent* GetCameraComp( ) const { return CachedCameraComp.Get(); }
	FORCEINLINE USpringArmComponent* GetSpringArmComp( ) const { return CachedSpringArmComp.Get(); }

	// Spring Arm의 길이를 조정
	void AddSpringArmLength( float InDeltaLength );

	// Spring Arm 길이 조정 속도를 설정
	void SetSpringArmResizeSpeed( float InSpeed ) { CachedSpringArmResizeSpeed = InSpeed; }

	// UR4CameraManageComponent를 초기화.
	virtual void Clear();
private:
	// Spring Arm을 Update
	bool _UpdateSpringArmLength( float InDeltaTime ) const;

private:
	// 관리중인 Camera Comp
	UPROPERTY( VisibleInstanceOnly, Transient, Category = "SpringArmUpdate" )
	TWeakObjectPtr<UCameraComponent> CachedCameraComp;

	// 관리중인 SpringArm Comp
	UPROPERTY( VisibleInstanceOnly, Transient, Category = "SpringArmUpdate" )
	TWeakObjectPtr<USpringArmComponent> CachedSpringArmComp;

	// Spring Arm Update 시 허용할 오차값
	UPROPERTY( EditAnywhere, Category = "SpringArmUpdate", meta = ( UIMin = "0.01", ClampMin = "0.01" ) )
	float SpringArmLengthTolerance;
	
	// Spring Arm Update 시 변해야하는 목표 값
	float CachedNewSpringArmLength;

	// Spring Arm Update 시 Speed
	float CachedSpringArmResizeSpeed;
};
