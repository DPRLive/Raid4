#pragma once

#include "../Core/SingletonBase.h"

/**
 * 싱글톤을 관리하는 Manager.
 * GameInstance를 통해 CDO에서 싱글톤을 한번 생성하고 Singleton의 데이터를 로드 / 정리 할 수 있도록 함.
 * (기존 Asset Manager Singleton은 그게 안됨)
 */
class FSingletonManager
{
private:
	struct FFuncs
	{
		TFunction<void()> InitFunc;
		TFunction<void()> ClearFunc;
		TFunction<void()> DestroyFunc;
	};

public:
	// 싱글톤들을 초기화한다.
	void InitSingletons()
	{
		for (FFuncs& funcs : SingletonFuncs)
		{
			funcs.InitFunc();
		}
	}

	// 싱글톤들을 정리한다.
	void ClearSingletons()
	{
		for (FFuncs& funcs : SingletonFuncs)
		{
			funcs.ClearFunc();
		}
	}

	// 필요시 싱글톤들을 해제한다.
	void DestroySingletons()
	{
		for (FFuncs& funcs : SingletonFuncs)
		{
			funcs.DestroyFunc();
		}

		SingletonFuncs.Empty();
	}

	// 싱글톤을 추가한다.
	template <typename Type>
	void AddSingleton(Type* InInstance)
	{
		FFuncs funcs;
		funcs.InitFunc = [InInstance]() { InInstance->InitSingleton(); };
		funcs.ClearFunc = [InInstance]() { InInstance->ClearSingleton(); };
		funcs.DestroyFunc = []() { Type::Destroy(); };

		SingletonFuncs.Emplace(MoveTemp(funcs));
	}

private:
	// 싱글톤을 관리할 수 있게 해주는 Lambda Funcs
	TArray<FFuncs> SingletonFuncs;
};

// 싱글톤 등록 매크로
#define ADD_SINGLETON(Name)									\
	SingletonManager.AddSingleton<Name>( Name::Create() );
