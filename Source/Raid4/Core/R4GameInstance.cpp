// Fill out your copyright notice in the Description page of Project Settings.


#include "R4GameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4GameInstance)

/**
 * 생성자
 */
UR4GameInstance::UR4GameInstance()
{
	_AddSingletons();
}

/**
 * 게임 시작시 호출
 */
void UR4GameInstance::Init()
{
	Super::Init();

	SingletonManager.InitSingletons();
}

/**
 * 게임 종료시 호출
 */
void UR4GameInstance::Shutdown()
{
	SingletonManager.ClearSingletons();
	
	Super::Shutdown();
}

/**
 * 싱글톤들을 등록한다.
 */
void UR4GameInstance::_AddSingletons()
{
	// ADD_SINGLETON(  );
}
