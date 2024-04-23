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
 *  Class Default Object를 기준으로 EnableTick, EnableColliison 여부를 결정 
 *  @param InUClass : 꺼내고 싶은 Class의 UClass. TSubclassOf<>로 감싸진 경우 그대로 넘겨야함!
 */
AActor* FActorPool::GetPoolActor(UClass* InUClass, const FTransform& InWorldTrans)
{
	//return _CreatePoolActor(InUClass);
	if(InUClass == nullptr)
	{
		LOG_ERROR(R4Log, TEXT("InUClass is nullptr."));
		return nullptr;
	}
	
	// reflection을 통한 type check
	if(!ensureMsgf(InUClass->IsChildOf<AActor>() && InUClass->ImplementsInterface(UR4ActorPoolable::StaticClass()),
	TEXT("ActorPoolable actor must inherit AActor and IR4ActorPoolable.")))
		return nullptr;
	
	AActor* retActor = _TryGetValidPoolActor(InUClass->GetFName());
	
	// 없는경우 새로 생성 후 리턴
	if(retActor == nullptr)
		retActor = _CreatePoolActor(InUClass);
	
	// Interface로 구현한 작업 처리
	if(IR4ActorPoolable* poolActor = Cast<IR4ActorPoolable>(retActor))
		poolActor->PreGetPoolActor();
	
	retActor->SetActorTransform(InWorldTrans);
	return retActor;
}

/**
 *  오브젝트를 Pool에 반납한다.
 *  반납 전 EnableCollison (false), HiddenInGame(true), SetActorTickEnabled(false) 작업이 기본으로 진행되나
 *  Actor의 bHidden 속성 말고는 Replicate 되지 않으므로 주의
 *  @return : nullptr = 없음 
 */
bool FActorPool::ReturnPoolActor(AActor* InPoolActor)
{
	if(!::IsValid(InPoolActor))
		return false;
	
	UClass* uClass = InPoolActor->GetClass();
	if(uClass == nullptr)
		return false;
	
	FName className = uClass->GetFName();
	
	// 반납 시도 전 자리가 없으면 비우기
	if(MaxPoolSize <= NowPoolSize)
		_TrimPool();
	
	// ActorPoolIter에서 관리되고 있는 경우에만 반납 성공
	if(auto it = ActorPoolIters.Find(className); it != nullptr && *it)
	{
		_DeactivateActor(InPoolActor);
	
		// Interface로 구현한 작업 처리
		if(IR4ActorPoolable* poolActor = Cast<IR4ActorPoolable>(InPoolActor))
			poolActor->PreReturnPoolActor();

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
		ActorPoolIters.Emplace(InClassName, ActorPool.GetHead());
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
			_ActivateActor(retActor);
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

/**
 *  Pool에서 꺼낸 Actor를 활성화
 *  @param InPoolActor : Pool에서 꺼낸 액터
 */
void FActorPool::_ActivateActor(AActor* InPoolActor)
{
	if(!::IsValid(InPoolActor))
		return;
	
	// TODO : Tick on, off 자식까지 전파 안되니 처리 필요 enable collision, hidden 도 자식까지 전파 안되나 .. ?
	UClass* uClass = InPoolActor->GetClass();
	if(uClass == nullptr)
		return;

	// CDO대로 Actor Tick과 Collision Enable
	if(AActor* defaultObj = uClass->GetDefaultObject<AActor>())
	{
		InPoolActor->SetNetDormancy(DORM_DormantAll);
		InPoolActor->SetActorHiddenInGame(defaultObj->IsHidden());
		InPoolActor->SetActorEnableCollision(defaultObj->GetActorEnableCollision()); // Component 까지 다 꺼버림
		InPoolActor->SetActorTickEnabled(defaultObj->CanEverTick());
	}
}

/**
 *  Pool에 반납할 Actor를 비활성화
 *  @param InPoolActor : Pool에 반납할 액터
 */
void FActorPool::_DeactivateActor(AActor* InPoolActor)
{
	// TODO : SetActorHiddenInGame + Collision을 disable하면 연관성 x로 client에서 actor가 destroy됨
	InPoolActor->SetActorHiddenInGame(true); // bHidden -> Replicate 됨
	InPoolActor->SetActorEnableCollision(false); // replicate 안됨
	InPoolActor->SetActorTickEnabled(false); // Replicate 안됨
	InPoolActor->SetNetDormancy(DORM_Awake);
}
