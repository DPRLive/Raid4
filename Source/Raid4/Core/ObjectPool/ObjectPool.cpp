#include "ObjectPool.h"
#include "PoolableInterface.h"
#include "PoolableActor.h"

void FObjectPool::InitSingleton()
{
	 
}

void FObjectPool::ClearSingleton()
{
	ClearObjectPool();
}

/**
 *  Pool을 정리.
 */
void FObjectPool::ClearObjectPool()
{
	PoolIters.Empty();

	// Pool 정리
	for(auto& [name, deque] : Pool)
	{
		while(!deque.IsEmpty())
		{
			TWeakObjectPtr<UObject> nowObj = deque.First();
			deque.PopFirst();

			if(!nowObj.IsValid())
				continue;

			// RootSet에 등록 된 UObject를 위한 Remove From RootSet
			if(nowObj->IsRooted())
				nowObj->RemoveFromRoot();
			
			// APoolableActor 기반인 경우 Destroy
			if(APoolableActor* poolActor = Cast<APoolableActor>(nowObj))
				poolActor->Destroy();
		}
	}

	Pool.Empty();
	
	NowPoolSize = 0;
}

/**
 *  해당 UClass에 맞는 Object를 pool에서 꺼내온다.
 *  @param InUClass : 꺼내고 싶은 Class의 UClass. TSubclassOf<>로 감싸진 경우 그대로 넘겨야함!
 */
UObject* FObjectPool::GetObject(UClass* InUClass)
{
	if(!::IsValid(InUClass))
	{
		LOG_ERROR(R4Log, TEXT("InUClass is nullptr."));
		return nullptr;
	}
	
	// reflection을 통한 type check
	if(!ensureMsgf(InUClass->ImplementsInterface(UPoolableInterface::StaticClass()), TEXT("Poolable Object must implement IPoolableInterface.")))
		return nullptr;

	UObject* retObj = _TryGetValidPoolObject(InUClass->GetFName());
	
	// 없는경우 새로 생성 후 리턴
	if(!::IsValid(retObj))
		retObj = _CreatePoolObject(InUClass);
	
	// Pool Actor를 사용하기 위한 사용자 로직 처리
	if(IPoolableInterface* poolObj = Cast<IPoolableInterface>(retObj))
		poolObj->PostInitPoolObject();
	
	return retObj;
}

/**
 *  오브젝트를 Pool에 반납한다.
 *  APoolableObject 상속 경우 반납 전 EnableCollison (false), HiddenInGame(true), SetActorTickEnabled(false) 작업이 기본으로 진행
 *	@param InPoolObject : 반납할 Object
 *  @return : 성공 여부
 */
bool FObjectPool::ReturnPoolObject(UObject* InPoolObject)
{
	if(!::IsValid(InPoolObject))
		return false;
	
	UClass* uClass = InPoolObject->GetClass();
	if(!::IsValid(uClass))
		return false;

	// 인터페이스 상속 안된거 막 반납하시면 안돼요
	if(!ensureMsgf(uClass->ImplementsInterface(UPoolableInterface::StaticClass()), TEXT("Do not implement IPoolableInterface object cannot be returned.")))
		return false;
	
	FName className = uClass->GetFName();
	
	// 반납 시도 전 자리가 없으면 비우기
	if(MaxPoolSize <= NowPoolSize)
		_TrimPool();
	
	// ActorPoolIter에서 관리되고 있는 경우에만 반납 성공
	if(auto it = PoolIters.Find(className); it != nullptr && *it)
	{
		// APoolableActor 기반인 경우 처리
		if(APoolableActor* poolActor = Cast<APoolableActor>(InPoolObject))
			poolActor->DeactivateActor();
		else
			InPoolObject->AddToRoot(); // 아니면 RootSet 등록
		
		// PoolableObject 를 반납하기 위한 사용자 로직 처리
		if(IPoolableInterface* poolObj = Cast<IPoolableInterface>(InPoolObject))
			poolObj->PreReturnPoolObject(); 

		(**it).Value.PushLast(InPoolObject);
		
		NowPoolSize++;
		
		return true;
	}

	// PoolSize가 꽉 차서 관리 해제 되었다거나 등 모종의 이유로 Pool에 의해 관리되지 않는 액터라면 그냥 Destroy
	
	// APoolableActor 기반인 경우 Destroy 호출. 아니면 알아서 GC가 가져감
	if(APoolableActor* poolActor = Cast<APoolableActor>(InPoolObject))
		poolActor->Destroy();
	
	return false;
}

/**
 *  Pool에서 유효한 Object를 찾아서 return
 *  @param InClassName : 찾고 싶은 Class의 이름. (UClass에 저장된 Class Name)
 *  @return : nullptr = 없음 
 */
UObject* FObjectPool::_TryGetValidPoolObject(FName InClassName)
{
	if ( !InClassName.IsValid() )
		return nullptr;
	
	auto it = PoolIters.Find(InClassName);
	if(it == nullptr || !(*it))
	{
		// TMap에 없거나 iter가 없어도 최근에 사용을 요청했으므로 일단 넣어는 둠
		Pool.AddHead({InClassName, TDeque<TWeakObjectPtr<UObject>>()});

		// 적당히 10개 정도 '용량' 확보
		auto head = Pool.GetHead();
		head->GetValue().Value.Reserve(10);
		
		PoolIters.Emplace(InClassName, head);
		return nullptr;
	}
	
	// 최근에 사용을 요청했으므로 맨 앞으로 이동
	Pool.RemoveNode(it->GetNode(), false);
	Pool.AddHead(it->GetNode());
	
	// 유효한 Actor를 찾기
	TDeque<TWeakObjectPtr<UObject>>& deque = (**it).Value;
	while (!deque.IsEmpty())
	{
		TWeakObjectPtr<UObject> poolObj = deque.First();
		deque.PopFirst();

		NowPoolSize--;

		if (poolObj.IsValid()) // 유효하면 return
		{
			// RootSet에 등록되어 있는경우 제거
			if(poolObj->IsRooted())
				poolObj->RemoveFromRoot();
			
			// 혹시 Actor 기반이면 처리 해줌
			if(APoolableActor* poolableActor = Cast<APoolableActor>(poolObj))
				poolableActor->ActivateActor();	// 활성화
			
			return poolObj.Get();
		}
	}
	
	return nullptr;
}

/**
 *  새로운 Pool Object를 만든다.
 *  @param InUClass : 만들고 싶은 Class의 UClass. TSubclassOf<>로 감싸진 경우 그대로 넘겨야함!
 */
UObject* FObjectPool::_CreatePoolObject(UClass* InUClass)
{
	if(::IsValid(InUClass) && InUClass->IsChildOf<APoolableActor>())
	{
		// Actor 기반이면 Spawn
		return R4GetWorld()->SpawnActor(InUClass);
	}

	return NewObject<UObject>((UObject*)GetTransientPackage(), InUClass);
}

/**
 *  Pool 크기 확보를 위해 PoolActor를 하나 이상 지움
 */
void FObjectPool::_TrimPool()
{
	// LRU 기법을 채택했으므로 가장 오래전에 사용된 Tail에서 하나 지움
	while(!Pool.IsEmpty())
	{
		if(auto tailNode = Pool.GetTail())
		{
			TDeque<TWeakObjectPtr<UObject>>& deque = tailNode->GetValue().Value;
			bool bTrimValidActor = false;
			
			while(!deque.IsEmpty() && !bTrimValidActor) // 유효한 액터를 지우기 전까진 반복
			{
				TWeakObjectPtr<UObject> poolObj = deque.First();
				deque.PopFirst();

				NowPoolSize--;
				
				if (poolObj.IsValid())
				{
					bTrimValidActor = true;

					// RootSet 에 등록되어 있는 경우 remove
					if(poolObj->IsRooted())
						poolObj->RemoveFromRoot();

					// Actor 기반인 경우 Destroy
					if(APoolableActor* poolActor = Cast<APoolableActor>(poolObj))
						poolActor->Destroy();
				}
			}
			
			// deque가 빌 때까지 지웠다면 ActorPoolIter, ActorPool에서도 제거
			if(deque.IsEmpty())
			{
				PoolIters.Remove(tailNode->GetValue().Key);
				Pool.RemoveNode(tailNode);
			}

			// 유효한 액터를 지웠다면 return
			if(bTrimValidActor)
				return;
		}
	}
}
