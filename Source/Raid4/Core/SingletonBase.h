#pragma once

/**
 * 싱글톤을 구현하기 위한 base 클래스 template
 */
template <typename Type>
class TSingletonBase
{
public:
	TSingletonBase() = default;

	virtual ~TSingletonBase();

	// 싱글톤 초기화 함수
	virtual void InitSingleton() = 0;

	// 싱글톤 정리 함수
	virtual void ClearSingleton() = 0;

	// 인스턴스 생성 함수
	static Type* Create();

	// 인스턴스 반환 함수
	static Type* Get();

	// 인스턴스 해제 함수
	static void Destroy();
	
	// 유효성 검사 함수
	static bool IsValid();
	
private:
	// 전역 인스턴스 (static을 클래스 내부에 옮기면 전역 변수를 namespace에 옮긴것과 비슷)
	static Type* Instance;
};

#include "SingletonBase.inl"