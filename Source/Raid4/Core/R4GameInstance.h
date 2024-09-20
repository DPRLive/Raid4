// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Engine/GameInstance.h>

#include "R4GameInstance.generated.h"

// singleton 선언 매크로
#define DECLARE_SINGLETON( Type, Name )			\
private:										\
	TUniquePtr<Type> Name;						\
public:											\
	const TUniquePtr<Type>& Get##Name() const	\
	{ return Name; }

class FDataTableManager;
class FObjectPool;

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
	// Singleton 처럼 1개의 인스턴스만 쓰기 위한 Object들 선언
	DECLARE_SINGLETON( FDataTableManager, DataTableManager )
	DECLARE_SINGLETON( FObjectPool, ObjectPool )
	
	// singleton들을 등록
	void _CreateSingletons();

	// singleton들을 정리
	void _ClearSingletons();
};

// Singleton 생성 매크로. _CreateSingletons() 내부에서 사용
#define CREATE_SINGLETON( Type, Name ) \
	Name = MakeUnique<Type>();		   \
	Name->InitSingleton();

// Singleton Clear 매크로. _ClearSingletons() 내부에서 사용
#define CLEAR_SINGLETON( Name ) \
	Name->ClearSingleton();		\
	Name.Reset();