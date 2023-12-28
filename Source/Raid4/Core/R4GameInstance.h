// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Manager/SingletonManager.h"
#include <Engine/GameInstance.h>
#include "R4GameInstance.generated.h"

/**
 * GameInstance
 */
UCLASS()
class RAID4_API UR4GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UR4GameInstance();
	
	// 게임 시작시 호출
	virtual void Init() override;

	// 게임 종료시 호출
	virtual void Shutdown() override;
	
private:
	// 싱글톤들을 등록한다.
	void _AddSingletons();
	
	// 싱글톤들을 관리해주는 SingletonManager
	FSingletonManager SingletonManager;
};
