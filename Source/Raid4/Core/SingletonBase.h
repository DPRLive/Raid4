#pragma once

/**
 * 싱글톤인듯 아닌듯한..을 위한 base 클래스.
 * 1개의 인스턴스를 유지하기 위해 이동 & 복사를 제거
 * 거의 GameInstance에 기생하도록 만드니, GameInstance가 유효할 때 사용할 것
 */
class FSingletonBase
{
public:
	FSingletonBase() = default;
	
	virtual ~FSingletonBase() = default;

	// 싱글톤 초기화 함수
	virtual void InitSingleton() = 0;

	// 싱글톤 정리 함수
	virtual void ClearSingleton() = 0;
	
public:
	// 이동 및 복사 불가.
	FSingletonBase( const FSingletonBase& ) = delete;
    FSingletonBase( FSingletonBase&& ) = delete;
    FSingletonBase& operator=( const FSingletonBase& ) = delete;
    FSingletonBase& operator=( FSingletonBase&& ) = delete;	
};