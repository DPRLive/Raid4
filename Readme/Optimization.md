
# 최적화

- 몇가지 간단한 최적화를 진행 해보았습니다.

## Object Pool
- 해당 프로젝트에서 설계하고 제작한 전투 시스템 프레임워크는, C++로 기반 시스템을 작성하고 BP로 확장 후 데이터를 설정하여 클래스 기반으로 다양한 버프, 스킬을 만들어내는 구조입니다. 이는 간단하고 쉽게 확장이 가능하나 BP 클래스가 많아지고, 런타임에 사용 시 빈번한 객체의 생성과 삭제가 반복된다는 단점이 있습니다.

- UObject를 Base로 하는 클래스들은 Unreal GC의 영향을 받게 되는데, Unreal GC는 삭제 예정 Object들을 대기시켰다가 한번에 삭제 (일반적으로 60초에 한번 씩) 시키는 구조로, 많은 UObject 클래스들을 빈번하게 생성 및 삭제 시 생성 뿐만 아니라 GC 동작 시에도 hitch를 발생시킬 수 있습니다.

- 따라서 객체의 잦은 생성 / 삭제가 예상되는 Object들에 대해서 Object Pool을 통한 관리로 최적화를 진행해보았습니다.

### FObjectPool

![image](https://github.com/user-attachments/assets/292a05b0-188e-4f27-b852-9b334c14f06a)

- Object Pool은 다음과 같이 사용할 수 있습니다.

#### UObject Base
	- IPoolableInterface를 상속 및 필요한 함수를 구현
	- 해당 객체가 필요 시 ObjectPool에게 GetObject()로 요청, ReturnPoolObject()로 반납.

#### AActor Base
	- APoolableActor를 상속 및 필요한 함수를 구현
	- 해당 객체가 필요 시 ObjectPool에게 GetObject()로 요청, ReturnPoolObject()로 반납.

#### Object Pool의 자료구조
- Object Pool을 설계할 때 중요한 고려사항 중 하나는 메모리 한계로 인해 무제한으로 Object를 캐싱할 수 없다는 점입니다. 따라서 Pool의 크기를 적절히 관리하는 것이 필수적이며, Pool의 크기를 초과했을 때 기존 Object를 어떤 방식으로 삭제할지도 중요한 결정 요소입니다.

- 저는 "자주 사용되는 데이터를 캐싱하지만, 메모리 제약으로 인해 효율적인 관리가 필요하다"는 개념이 운영체제의 페이징 교체와 유사하다고 생각했습니다. 이에 운영체제의 페이징 교체 알고리즘 중 성능이 우수한 편인 LRU(Least Recently Used) 알고리즘을 선택하여 Object Pool에 적용해보았습니다.

- LRU는 Object Pool에서 가장 오랫동안 사용되지 않은 객체를 교체함으로써 자주 사용되는 객체는 더 오래 캐시에 남아 있게 해줍니다. 이 프로젝트에서 Object Pool을 적용할 주요 대상은 Skill의 Detect와 Buff이며, 이 두 시스템은 전투에서 빈번하게 호출되고 특히 쿨타임이 짧거나 플레이어가 선호하는 스킬에서는 더욱 자주 사용될 가능성이 아주 높습니다.

- 따라서, 캐시의 크기를 제한하고 빈번한 객체 교체를 효율적으로 관리하기 위해 LRU 알고리즘을 적용했습니다. 이로 인해 불필요한 객체 삭제를 줄이고, 객체 재사용을 극대화하여 성능을 향상시키고자 했습니다.

- '사용되었다'의 기준은 'Pool에서 꺼내 갔을때의 시점'으로 설정하였습니다.

- ObjectPool의 내부는 다음과 같이 관리됩니다.

![image](https://github.com/user-attachments/assets/f229dd92-ff7e-460e-9291-f8ef82b41619)

- Object들은 TDoubleLinkedList로 관리되며, { UClass의 이름, Instance들을 저장할 Deque }를 노드로 가집니다.

- 해당 Object의 UClass 이름과, Object를 저장하는 TDoubleLinkedList의 Node를 가리키는 Iterator를 TMap에 캐싱합니다.

[[ObjectPool.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Core/ObjectPool/ObjectPool.h)

```
	// ObjectPool의 Node가 될 타입
	using FObjectPoolNode = TPair<FName, TDeque<TWeakObjectPtr<UObject>>>;
	
	// Object Pool에 무엇이 들어있는지 관리
	TMap<FName, TDoubleLinkedList<FObjectPoolNode>::TIterator> PoolIters; 
	
	// TQueue를 쓰려했지만 복사도 안돼 Num()도 없어 DoubleLinkedList AddHead는 const & 만 인자로 받는.. 등의 이유로 Deque사용
	// Pool 크기 관리 시 LRU 방식을 채택하기 위해 DoubleLinkedList 사용
	TDoubleLinkedList<FObjectPoolNode> Pool;
```

#### Instance 요청
- ObjectPool에게 UClass*를 인자로 Object를 요청하면, TMap에서 해당 UClass의 이름을 통해 Find하여 해당 Object에 대한 관리가 현재 이루어지고 있는지 확인합니다.

- TMap에 있다면 LinkedList의 Node에 바로 접근하며, 없는 경우 새로 Node를 생성합니다.

- 최근에 요청되었으니 사용되었다고 판단, Node를 'LinkedList의 맨 앞'으로 이동합니다.

- 해당 Node의 맨 위부터 Deque를 확인하며 유효한 객체가 있는지 찾고, 있다면 Deque에서 Pop하여 Return, 없다면 새로 생성하여 Return합니다.

- 이때, Deque에서 객체를 꺼내서 Return한다면, Pool에 반납 시 RootSet에 등록했을 수 있으므로 RootSet 등록을 해제해줍니다.

[[ObjectPool.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Core/ObjectPool/ObjectPool.cpp)

```
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
```
#### Instance 반납
- 반납 시도 전, Pool의 Size가 MaxPoolSize에 도달하였다면 기존 Pool에서 관리되던 Object 중 가장 오래된 Object를 삭제하여 공간을 비웁니다.

- Linkedlist의 Tail에 접근하여 (사용할 때 Object는 Head로 이동시켰기 때문에, Tail에 있다는 뜻은 사용된 지 오래되었다는 뜻) Instance를 제거합니다. 이때, 유효하지 않은 Instance가 발견되면 같이 제거합니다.

- Instance를 저장하는 용도의 Deque가 모두 비워질 때까지 지워지면, Node를 캐싱하는 TMap에서도 제거하여 더 이상 관리하지 않도록 합니다.

- 다시 돌아와서, 반납을 시도했던 Object를 TMap에서 UClass의 이름을 통해 현재 Pool에서 관리되고 있는 객체인지 확인하고, 관리중이라면 Node에 접근 후 Deque에 추가하여 반납합니다.

- 이때, GC 참조 카운트가 0이 될 수 있으므로 GC에 의해 Collect 당하지 않도록 RootSet에 등록해줍니다.

[[ObjectPool.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Core/ObjectPool/ObjectPool.cpp)

```
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

```

#### APoolableActor

- 기본적으로 Object Pool에 관리를 받으려는 Object는 IPoolableInterface를 상속하여 꺼낸 후 / 반납 전 로직을 처리할 수 있습니다.

- AActor Base의 경우에는 레벨에 배치되기 때문에 반납시 SetNetDormancy(), Actor Hidden, Disable Collision, Disable Tick 작업, 다시 사용하려고 Get시 CDO에 설정된대로 상태를 복구하는 작업이 추가적으로 진행되어야 하기에, Abstract Class를 만들어 해당 작업이 자동으로 진행될 수 있도록 하였습니다.

- 또한 AActor의 경우 Replicated Actor가 bHidden && Disable Collision 으로 설정 시 Relevant 체크에 실패하여 Client Side에서 Destroy되는 문제가 발생하였습니다.

- 이를 해결하기 위해 IsNetRelevantFor를 Override하고 bHidden && Disable Collision 일 경우에 Relevant를 가지지 않도록 하는 if문을 제거하여 Client에서도 Instance가 제거되지 않고 휴면 상태로 유지될 수 있도록 하였습니다.

[[PoolableActor.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Core/ObjectPool/PoolableActor.cpp)

```
/**
 *  Server에서 bHidden && Disable Collision 설정 시 Client side에서 Destroy 당하지 않도록
 *  bHidden && Disable Collision 일 경우에 Relevant를 가지지 않도록 하는 if문을 제거
 */
bool APoolableActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	if (bAlwaysRelevant || IsOwnedBy(ViewTarget) || IsOwnedBy(RealViewer) || this == ViewTarget || ViewTarget == GetInstigator())
	{
		return true;
	}
	if (bNetUseOwnerRelevancy && Owner)
	{
		return Owner->IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	}
	if (bOnlyRelevantToOwner)
	{
		return false;
	}
	if (RootComponent && RootComponent->GetAttachParent() && RootComponent->GetAttachParent()->GetOwner() && (Cast<USkeletalMeshComponent>(RootComponent->GetAttachParent()) || (RootComponent->GetAttachParent()->GetOwner() == Owner)))
	{
		return RootComponent->GetAttachParent()->GetOwner()->IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	}

	if (!RootComponent)
	{
		UE_LOG(LogNet, Warning, TEXT("Actor %s / %s has no root component in AActor::IsNetRelevantFor. (Make bAlwaysRelevant=true?)"), *GetClass()->GetName(), *GetName() );
		return false;
	}

	return !GetDefault<AGameNetworkManager>()->bUseDistanceBasedRelevancy ||
			IsWithinNetRelevancyDistance(SrcLocation);
}

/**
 *  Actor를 Pool에 반납하기 위해 비활성화
 */
void APoolableActor::DeactivateActor()
{
	if(HasAuthority())
	{
		// Replicate 휴면 액터로 변경
		SetNetDormancy(DORM_DormantAll);
		FlushNetDormancy();
		
		SetActorHiddenInGame(true);
		DisableCollisionAndTick();
		
		bActivate = false;
	}
}

/**
 *  Actor를 Pool에서 꺼내기 위해 활성화
 */
void APoolableActor::ActivateActor()
{
	if(HasAuthority())
	{
		// Replicate 휴먼 해제
		SetNetDormancy(DORM_Awake);
		SetActorHiddenInGame(false);
		ResetCollisionAndTick();

		bActivate = true;
	}
}

/**
 *  Collision과 Tick을 자식 컴포넌트까지 모두 비활성화
 *  Actor의 bHidden 속성 말고는 Replicate 되지 않아 직접 Replicate
 */
void APoolableActor::DisableCollisionAndTick()
{
	if ( bControlCollisionByPool )
		SetActorEnableCollision(false); // Component 까지 알아서 꺼버림

	SetActorTickEnabled(false);

	TArray<UActorComponent*> comps;
	GetComponents(comps);
	
	for(UActorComponent* comp : comps)
		comp->SetComponentTickEnabled(false);
}

/**
 *  Collision과 Tick을 자식 컴포넌트까지 원래 CDO (UCLASS) 대로 복구
 */
void APoolableActor::ResetCollisionAndTick()
{
	UClass* uClass = GetClass();
	if(!IsValid(uClass))
		return;
	
	if(const AActor* cdo = uClass->GetDefaultObject<AActor>(); IsValid(cdo))
	{
		if ( bControlCollisionByPool )
			SetActorEnableCollision(cdo->GetActorEnableCollision());
		SetActorTickEnabled(cdo->CanEverTick());
	}

	TArray<UActorComponent*> comps;
	GetComponents(comps);
	for(UActorComponent* comp : comps)
	{
		UClass* compUClass = comp->GetClass();
		if(!IsValid(compUClass))
			continue;
		
		if(const UActorComponent* cdo = compUClass->GetDefaultObject<UActorComponent>(); IsValid(cdo))
			comp->SetComponentTickEnabled(cdo->PrimaryComponentTick.bCanEverTick);
	}
}
```

#### 최적화 결과

- Object Pool을 구성후, 간단한 테스트를 통해 성능을 측정해보았습니다.

```
- Listen Server, Server 1 Client 1
- 서로 다른 Actor Class 10개를 준비
- '10개의 Class들을 무작위로 뽑아 300개 생성'을 2초에 한번씩 10회 실행
- 생성된 Actor는 0 ~ 5초 사이의 랜덤 LifeTime을 가짐
- Object Pool 사용 시 Pool의 Size는 500
```

- 약 20회 테스트 결과 편차가 있었으나, 평균적으로 다음과 같은 결과를 얻었습니다.

#### 걸린 시간 비교
```
- 첫 Actor 생성시 : PoolActor > Normal Actor
- Pool에 Instance가 있는 경우 Actor 생성 시 : PoolActor < Normal Actor
- Pool에 Instance가 비어있는 경우 Actor 생성 시 : PoolActor > Normal Actor 
- Pool의 Size가 가득 찬 경우 반납 시 (Normal은 Destroy): PoolActor > Normal Actor
- GC 동작시 : PoolActor < Normal Actor
```

- 아래는 테스트 결과의 일부입니다. (좌 : Normal Actor, 우 : Pool Actor)

![image](https://github.com/user-attachments/assets/d6d64e62-58f9-44e9-bc79-eaa4816a9d70)


- 테스트는 1 Tick에 300개의 Actor를 요청하는 극단적인 상황에서 진행되었습니다. 이로 인해 Pool이 비어있는 경우가 많아 큰 성능 차이는 없었으나, Object Pool에 인스턴스가 미리 준비된 상태에서는 객체 요청 시 더 유리한 성능을 보여주었고, Pool이 비어있는 경우에는 추가적인 작업으로 인해 일반적인 객체 생성보다 시간이 더 소요되는 결과가 나타났습니다.

- GC가 동작할 때는 Pool에 객체가 캐싱되어 있어 Object Pool이 더욱 유리한 성능을 보였습니다.

- Pool이 비어있을 때 성능 저하가 발생했기에, Pool의 크기를 적절히 조절하는 것이 중요하다고 판단했습니다. 이를 바탕으로, 4명의 캐릭터 + 보스 몬스터, 그리고 각 캐릭터의 스킬 4개에 대해 스킬당 넉넉히 5개의 Pool 객체를 사용한다고 가정해, 최종적으로 패키징시에는 Pool 크기를 100으로 설정하였습니다.

## Tick
- Tick 사용의 최소화는 Unreal Engine에서도 적극 권장하는 최적화 방식중 한개입니다.

- 따라서 Tick을 필요한 경우에만 사용하고, Tick이 필요한 경우 On / Off 및 가능하면 Tick Rate를 조절하는 방식으로 Tick의 사용을 최소화 하였습니다.

- 아래는 예시로 Tick의 Rate를 조절하고 동적으로 On / Off 한 Component중 하나인 UR4BuffManageComponent 입니다.

[[R4BuffManageComponent.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Buff/R4BuffManageComponent.cpp)

```
UR4BuffManageComponent::UR4BuffManageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// 적당히 갱신시간 타협
	SetComponentTickInterval( Buff::G_BuffTickInterval );

	SetIsReplicatedByDefault( true );
}

void UR4BuffManageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float nowServerTime = R4GetServerTimeSeconds( GetWorld() );
	if ( nowServerTime < 0.f )
	{
		LOG_WARN( R4Data, TEXT("nowServerTime is invalid.") );
		return;
	}
	
	// 버프 업데이트, 업데이트 할 버프
	bool bNeedUpdate = _UpdateBuffs( nowServerTime );

	// Update가 필요한 Buff가 더 이상 없다면 Tick Off
	if ( !bNeedUpdate )
		SetComponentTickEnabled( false );
}
```