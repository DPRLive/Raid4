#include "ActorPool.h"

void FActorPool::InitSingleton()
{
	
}

void FActorPool::ClearSingleton()
{
	ActorPoolIters.Empty();
	ActorPool.Empty();
}

/**
 *  해당 UClass에 맞는 Actor를 pool에서 꺼내온다.
 *  @param InUClass : 꺼내고 싶은 Class의 UClass. TSubclassOf<>로 감싸진 경우 그대로 넘겨야함!
 */
AActor* FActorPool::GetPoolActor(UClass* InUClass, const FTransform& InWorldTrans)
{
	if(!::IsValid(InUClass))
	{
		LOG_ERROR(R4Log, TEXT("InUClass is nullptr."));
		return nullptr;
	}
	
	// reflection을 통한 type check
	if(!ensureMsgf(InUClass->IsChildOf<APoolableActor>(), TEXT("ActorPoolable actor must inherit Abstract Class APoolableActor.")))
		return nullptr;
	
	AActor* retActor = _TryGetValidPoolActor(InUClass->GetFName());
	
	// 없는경우 새로 생성 후 리턴
	if(!::IsValid(retActor))
		retActor = _CreatePoolActor(InUClass);
	
	// PoolableActor를 사용하기 위한 사용자 로직 처리
	if(APoolableActor* poolActor = Cast<APoolableActor>(retActor))
		poolActor->PostGetPoolActor();
	
	retActor->SetActorTransform(InWorldTrans);
	return retActor;
}

/**
 *  오브젝트를 Pool에 반납한다.
 *  반납 전 EnableCollison (false), HiddenInGame(true), SetActorTickEnabled(false) 작업이 기본으로 진행
 *  @return : nullptr = 없음 
 */
bool FActorPool::ReturnPoolActor(AActor* InPoolActor)
{
	if(!::IsValid(InPoolActor))
		return false;
	
	UClass* uClass = InPoolActor->GetClass();
	if(!::IsValid(uClass))
		return false;
	
	FName className = uClass->GetFName();
	
	// 반납 시도 전 자리가 없으면 비우기
	if(MaxPoolSize <= NowPoolSize)
		_TrimPool();
	
	// ActorPoolIter에서 관리되고 있는 경우에만 반납 성공
	if(auto it = ActorPoolIters.Find(className); it != nullptr && *it)
	{
		if(APoolableActor* poolActor = Cast<APoolableActor>(InPoolActor); ::IsValid(poolActor))
		{
			poolActor->DeactivateActor();			
			poolActor->PreReturnPoolActor(); // PoolableActor를 반납하기 위한 사용자 로직 처리
		}

		(**it).Value.PushLast(InPoolActor);
		
		NowPoolSize++;
		
		return true;
	}

	// PoolSize가 꽉 차서 관리 해제 되었다거나 등 모종의 이유로 Pool에 의해 관리되지 않는 액터라면 그냥 Destroy
	InPoolActor->Destroy();
	return false;
}

/**
 *  Pool에서 유효한 Actor를 찾아서 return
 *  @param InClassName : 찾고 싶은 Class의 이름. (UClass에 저장된 Class Name)
 *  @return : nullptr = 없음 
 */
AActor* FActorPool::_TryGetValidPoolActor(FName InClassName)
{
	auto it = ActorPoolIters.Find(InClassName);
	if(it == nullptr || !(*it))
	{
		// TMap에 없거나 iter가 없어도 최근에 사용을 요청했으므로 일단 넣어는 둠
		ActorPool.AddHead({InClassName, TDeque<TWeakObjectPtr<AActor>>()});

		// 적당히 10개 정도 용량 확보
		auto head = ActorPool.GetHead();
		head->GetValue().Value.Reserve(10);
		
		ActorPoolIters.Emplace(InClassName, head);
		return nullptr;
	}
	
	// 최근에 사용을 요청했으므로 맨 앞으로 이동
	ActorPool.RemoveNode(it->GetNode(), false);
	ActorPool.AddHead(it->GetNode());
	
	// 유효한 Actor를 찾기
	TDeque<TWeakObjectPtr<AActor>>& deque = (**it).Value;
	while (!deque.IsEmpty())
	{
		TWeakObjectPtr<AActor> poolActor = deque.First();
		deque.PopFirst();

		NowPoolSize--;
		
		if (poolActor.IsValid()) // 유효하면 return
		{
			AActor* retActor = poolActor.Get();
			
			if(APoolableActor* poolableActor = Cast<APoolableActor>(retActor))
			{
				poolableActor->ActivateActor();	// 활성화
			}
			
			return retActor;
		}
	}
	
	return nullptr;
}

/**
 *  새로운 Pool Actor를 만든다.
 *  @param InUClass : 만들고 싶은 Class의 UClass. TSubclassOf<>로 감싸진 경우 그대로 넘겨야함!
 */
AActor* FActorPool::_CreatePoolActor(UClass* InUClass)
{
	return R4GetWorld()->SpawnActor(InUClass);
}

/**
 *  Pool 크기 확보를 위해 PoolActor를 하나 이상 지움
 */
void FActorPool::_TrimPool()
{
	// LRU 기법을 채택했으므로 가장 오래전에 사용된 Tail에서 하나 지움
	while(!ActorPool.IsEmpty())
	{
		if(auto tailNode = ActorPool.GetTail())
		{
			TDeque<TWeakObjectPtr<AActor>>& deque = tailNode->GetValue().Value;
			bool bTrimValidActor = false;
			
			while(!deque.IsEmpty() && !bTrimValidActor) // 유효한 액터를 지우기 전까진 반복
			{
				TWeakObjectPtr<AActor> poolActor = deque.First();
				deque.PopFirst();

				NowPoolSize--;
				
				if (poolActor.IsValid())
					bTrimValidActor = true;
			}
			
			// deque가 빌 때까지 지웠다면 ActorPoolIter, ActorPool에서도 제거
			if(deque.IsEmpty())
			{
				ActorPoolIters.Remove(tailNode->GetValue().Key);
				ActorPool.RemoveNode(tailNode);
			}

			// 유효한 액터를 지웠다면 return
			if(bTrimValidActor)
				return;
		}
	}
}
