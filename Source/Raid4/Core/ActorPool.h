#pragma once

#include "SingletonBase.h"
#include "../Interface/R4ActorPoolable.h"

#include <Containers/Deque.h>

class IR4ActorPoolable;

// ActorPool에 들어올 수 있는 타입을 제한하는 Concepts
template<typename T>
concept CPoolActor = (std::is_base_of_v<AActor, T> && std::is_base_of_v<IR4ActorPoolable, T>) && !TIsSame<IR4ActorPoolable, T>::Value;

// ActorPoll의 Node가 될 타입
using FActorPoolNode = TPair<FName, TDeque<TWeakObjectPtr<AActor>>>;

/**
 * AActor 기반 클래스를 Pooling 하기 위한 Object pool
 * 현재 Level에서 사용되고 있지 않는 상태만 Pool에 등록
 * Actor는 Level에서 따로 참조되므로, GC 참조 카운트를 증가시키지 않고 weak ptr로 참조함!'
 * Pool Size 관리 정책은 LRU를 따름. '사용되었다'의 기준은 'Pool에서 꺼내 갔을때의 시점'을 얘기함
 * TODO : Actor Pool Size limit & Management, Actor Init & return logic 
 */
class FActorPool : public TSingletonBase<FActorPool>
{
public:
	// 싱글톤 초기화 함수
	virtual void InitSingleton() override;

	// 싱글톤 정리 함수
	virtual void ClearSingleton() override;
	
	// T타입의 오브젝트를 Pool에서 꺼내온다.
	template<CPoolActor T>
	T* GetPoolActor(const FTransform& InWorldTrans);

	// 해당 UClass에 맞는 Actor를 pool에서 꺼내온다.
	AActor* GetPoolActor(UClass* InUClass, const FTransform& InWorldTrans);

	// Actor를 반납한다.
	bool ReturnPoolActor(AActor* InPoolActor);
	
private:
	// Pool에서 유효한 Actor를 찾아서 return
	AActor* _TryGetValidPoolActor(FName InClassName);
	
	// 새로운 Pool Actor를 만든다
	AActor* _CreatePoolActor(UClass* InUClass);

	// Pool 크기 확보를 위해 PoolActor를 하나 이상 지움
	void _TrimPool();
	
	// Pool에서 꺼낸 Actor를 활성화
	void _ActivateActor(AActor* InPoolActor);

	// Pool에 넣기 위해 Actor를 비활성화
	void _DeactivateActor(AActor* InPoolActor);
private:
	// Actor Pool의 최대 size를 관리
	uint32 MaxPoolSize = 1000;

	// 현재 Pool의 Size
	uint32 NowPoolSize = 0;
	
	// Actor Pool에 무엇이 들어있는지 관리
	TMap<FName, TDoubleLinkedList<FActorPoolNode>::TIterator> ActorPoolIters; 

	// TQueue를 쓰려했지만 복사도 안돼 Num()도 없어 DoubleLinkedList AddHead는 const & 만 인자로 받는.. 등의 이유로 Deque사용
	// TODO : Reserve하면 좋겠어.
	// Actor pool, Actor Pool 크기 관리 시 LRU 방식을 채택하기 위해 DoubleLinkedList 사용
	TDoubleLinkedList<FActorPoolNode> ActorPool;
};

/**
 *  T타입의 오브젝트를 Pool에서 꺼내온다.
 *  @return : nullptr = 없음 
 */
template <CPoolActor T>
T* FActorPool::GetPoolActor(const FTransform& InWorldTrans)
{
	UClass* uClass = T::StaticClass();
	if(uClass == nullptr)
	{
		LOG_ERROR(R4Log, TEXT("T::StaticClass is nullptr."));
		return nullptr;
	}

	AActor* retActor = _TryGetValidPoolActor(uClass->GetFName());

	// 없는경우 새로 생성 후 리턴
	if(retActor == nullptr)
		retActor = _CreatePoolActor(uClass);
	
	// Interface로 구현한 작업 처리
	if(IR4ActorPoolable* poolActor = Cast<IR4ActorPoolable>(retActor))
		poolActor->PreGetPoolActor();

	retActor->SetActorTransform(InWorldTrans);
	return Cast<T>(retActor);
}