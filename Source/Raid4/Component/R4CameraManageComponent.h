
#pragma once

#include <Components/ActorComponent.h>
#include "R4CameraManageComponent.generated.h"

/**
 *  Player의 Camera를 관리해주는 컴포넌트
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RAID4_API UR4CameraManageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UR4CameraManageComponent();
	
	// 컴포넌트 초기화
	virtual void InitializeComponent() override;
	
protected:
	virtual void BeginPlay() override;
	
};
