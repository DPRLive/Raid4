// Fill out your copyright notice in the Description page of Project Settings.


#include "R4GameInstance.h"

#include "../Manager/DataTableManager.h"
#include "ObjectPool/ObjectPool.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(R4GameInstance)

/**
 * 생성자
 */
UR4GameInstance::UR4GameInstance()
{

}

/**
 * 게임 시작시 호출
 */
void UR4GameInstance::Init()
{
	Super::Init();

	_CreateSingletons();
}

/**
 * 게임 종료시 호출
 */
void UR4GameInstance::Shutdown()
{
	_ClearSingletons();
	
	Super::Shutdown();
}

/**
 * singleton들을 등록
 */
void UR4GameInstance::_CreateSingletons()
{
	CREATE_SINGLETON( FDataTableManager, DataTableManager );
	CREATE_SINGLETON( FObjectPool, ObjectPool )
}

/**
 * singleton들을 정리
 */
void UR4GameInstance::_ClearSingletons()
{
	CLEAR_SINGLETON( DataTableManager )
	CLEAR_SINGLETON( ObjectPool )
}
