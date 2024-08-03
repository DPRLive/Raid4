#pragma once

#include "../SingletonBase.h"

#include <Containers/Deque.h>

/**
 * UObject 기반 클래스를 Pooling 하기 위한 Object pool
 * UObject 기반 시 IPoolableInterface를 구현, Actor의 경우 PoolableActor를 상속 후 구현!
 * Component의 경우는 실험이 되지 않았으므로 일단 UObject와 PoolableActor만 권장
 * UObject 기반 시 RootSet에 등록하여 GC를 막고, Actor의 경우는 Level에서 참조되므로 GC를 신경쓰지 않음
 * Pool Size 관리 정책은 LRU를 따름. '사용되었다'의 기준은 'Pool에서 꺼내 갔을때의 시점'을 얘기함
 */
class FObjectPool : public TSingletonBase<FObjectPool>
{
public:
	// 싱글톤 초기화 함수
	virtual void InitSingleton() override;

	// 싱글톤 정리 함수
	virtual void ClearSingleton() override;

	// Pool을 정리
	void ClearObjectPool();
	
	// 해당 UClass에 맞는 Object를 Pool에서 꺼내온다.
	UObject* GetObject(UClass* InUClass);
	
	// Object를 반납한다.
	bool ReturnPoolObject(UObject* InPoolObject);
	
private:
	// Pool에서 유효한 Object를 찾아서 return
	UObject* _TryGetValidPoolObject(FName InClassName);

	// 새로운 Pool Object를 만든다.
	UObject* _CreatePoolObject(UClass* InUClass);

	// Pool 크기 확보를 위해 PoolActor를 하나 이상 지움
	void _TrimPool();
	
private:
	// Actor Pool의 최대 size를 관리
	uint16 MaxPoolSize = GObjectPoolSize;

	// 현재 Pool의 Size
	uint16 NowPoolSize = 0;

	// ObjectPool의 Node가 될 타입
	using FObjectPoolNode = TPair<FName, TDeque<TWeakObjectPtr<UObject>>>;
	
	// Object Pool에 무엇이 들어있는지 관리
	TMap<FName, TDoubleLinkedList<FObjectPoolNode>::TIterator> PoolIters; 
	
	// TQueue를 쓰려했지만 복사도 안돼 Num()도 없어 DoubleLinkedList AddHead는 const & 만 인자로 받는.. 등의 이유로 Deque사용
	// Pool 크기 관리 시 LRU 방식을 채택하기 위해 DoubleLinkedList 사용
	TDoubleLinkedList<FObjectPoolNode> Pool;

};