# 데이터 기반 멀티플레이어 전투 시스템 프레임워크
- 스킬이나 버프 등 다양한 전투 관련 로직을 구성할 때, C++ 클래스의 과도한 확장을 줄이고, 데이터의 조정과 구성만으로 다양한 형태로 동작할 수 있는 구조가 필요함을 느꼈습니다.

- 따라서, 기본적으로 C++ 클래스를 사용하여 시스템을 구축한 후, Unreal Engine BP 에디터에서 해당 클래스를 확장후 데이터를 설정 및 조합하여 전투 시스템을 구성할 수 있는 데이터 기반 프레임워크를 설계하였습니다.

- 이 프레임워크는 크게 두 가지로 나누어집니다.
  - 다양한 효과를 적용하고 예외를 처리하는 Buff System
  - Buff System, Animation, Detect System( Shape Component, Trace 등 )를 조합한 Skill System

------------
## Buff System
- 전투 시스템에서 다양한 효과를 적용하고 예외를 처리하는 Buff 시스템입니다.

- 여기서 '효과'는 Stat 증감, 방어막 증감등 단순한 효과뿐만이 아닌, 강제 이동(루트 모션 대체 기능), Camera 조정 등 여러 예외 처리까지 포함하며, 이는 UR4BuffBase를 확장하여 처리할 수 있습니다.

### UR4BuffBase
![image](https://github.com/user-attachments/assets/8c763d5b-5cca-450c-8492-a5d8943c8120)

- Buff는 '어떠한 효과를 적용한다'는 역할을 유지하면서, 실제 적용되는 효과는 다양하게 확장될 수 있어야 했습니다. 이를 위해 '효과 적용'의 특정 단계만 확장 가능하게 하고, 전체 알고리즘 구조는 변경할 수 없도록 Template Method 패턴을 선택했습니다.

- 이를 위해 Abstract Class인 UR4BuffBase를 제작하였으며, 상속을 받은 C++ 클래스에서 버프가 적용할 '효과'를 재정의하는 방식으로 확장하도록 하였습니다.

- 어떠한 '효과'를 적용할 지 재정의한 C++ Buff 클래스는, 재정의된 효과를 적용할 때 수치가 필요할 수 있습니다. 이런 경우 BP로 확장해서 수치를 입력할 수 있고, 다양한 버프를 만들어 낼 수 있습니다.

- Buff가 적용할 '효과' 로직을 설정하기 위해, UR4BuffBase를 확장 시 아래와 같이 총 5단계를 확장할 수 있도록 하였습니다. <br>
[[UR4BuffBase.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Buff/R4BuffBase.h) / [[UR4BuffBase.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Buff/R4BuffBase.cpp)   
```
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의. 세팅( 버프 효과 적용이 가능한 상태인가 ) 실패 시 false를 꼭 리턴,
	// 클래스 상속 시 추가 정보가 필요하다면 오버라이드 하여 세팅 작업을 추가적으로 진행
	virtual bool SetupBuff(AActor* InInstigator, AActor* InVictim);
	
	// 버프를 적용, 실제로 해당 Buff가 적용할 효과 로직을 정의. SetupBuff 후 사용.
	virtual bool ApplyBuff() { return true; }
	
	// 버프를 제거 시 해야할 로직을 정의.
	virtual void RemoveBuff();

protected:
	// 버프 해제 시 Deactivate (버프가 한 짓 되돌리기)가 필요하다면 해야할 로직을 정의
	virtual void Deactivate() {}

	// 해당 버프 클래스를 초기 상태로 Reset
	virtual void Reset();
```
- 다음은 실제로 확장한 예시입니다. ( 데미지 적용 버프 ) <br>
[[R4Buff_DamageApplier.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Buff/Damage/R4Buff_DamageApplier.h)   
```
	public:
	// 버프가 적용 전 해야 할 로직 (세팅 등)해야 하는 것을 정의. 세팅( 버프 효과 적용이 가능한 상태인가 ) 실패 시 false를 꼭 리턴,
	// 클래스 상속 시 추가 정보가 필요하다면 오버라이드 하여 세팅 작업을 추가적으로 진행
	virtual bool SetupBuff(AActor* InInstigator, AActor* InVictim) override;
	
	// 버프를 적용, 실제로 해당 Buff가 적용할 효과 로직을 정의. SetupBuff 후 사용.
	virtual bool ApplyBuff() override;

private:
	// 가할 데미지 관련 정보를 설정.
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FR4DamageApplyDesc DamageApplyDesc;
``` 
[[R4Buff_DamageApplier.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Buff/Damage/R4Buff_DamageApplier.cpp)   
```
	/**
 *  버프 적용 전 세팅
 *  @param InInstigator : 버프를 시전한 액터
 *  @param InVictim : 버프를 적용할 대상
 *  @return : 세팅 성공 실패 여부
 */
bool UR4Buff_DamageApplier::SetupBuff(AActor* InInstigator, AActor* InVictim)
{
	bool bReady = Super::SetupBuff(InInstigator, InVictim);
	
	if(!InVictim->GetClass())
		return false;
	
	// 상대가 데미지를 받을 수 있는 객체인지 확인
	return bReady && InVictim->GetClass()->ImplementsInterface(UR4DamageReceiveInterface::StaticClass());
}

/**
 *  버프를 적용 ( 데미지를 적용 )
 */
bool UR4Buff_DamageApplier::ApplyBuff()
{
	if(!Super::ApplyBuff())
		return false;

	// 데미지 인터페이스를 통해 데미지 전달
	if(IR4DamageReceiveInterface* victim = Cast<IR4DamageReceiveInterface>(CachedVictim))
	{
		// 데미지 계산
		FR4DamageReceiveInfo damageInfo = UtilDamage::CalculateDamageReceiveInfo(CachedInstigator.Get(), CachedVictim.Get(), DamageApplyDesc);

		// 음수의 데미지를 가하려는 경우 경고
		if(damageInfo.IncomingDamage < 0.f)
			LOG_WARN(R4Log, TEXT("Warning! [%s] : Try to apply negative damage[%f]."), *GetName(), damageInfo.IncomingDamage);
		
		victim->ReceiveDamage(CachedInstigator.Get(), damageInfo);

		return true;
	}

	return false;
}
```
- 위 처럼 데미지를 적용하는 효과를 가진 Buff를 제작하고, 데미지에 대한 데이터는 BP에서 확장하여 다양한 데미지 Buff를 제작할 수 있습니다.
<img src="https://github.com/user-attachments/assets/d2adaac6-6a71-4839-9593-7ce47aaf2b72" width="300" height="400"/>

- 아래는 제가 직접 4개의 캐릭터를 제작하며 만든 C++ Buff 기반들과, 해당 기반 클래스를 BP에서 확장 후 데이터를 설정하여 다양하게 만들어낸 Buff 클래스들 입니다.

- [Buffs](https://github.com/DPRLive/Raid4/tree/master/Source/Raid4/Buff)  ( C++ 기반 )
- BP로 확장되어 데이터가 설정된 Buffs
<img src="https://github.com/user-attachments/assets/6f80ccbb-d251-46c2-8a91-f10ac7219143" width="750" height="400"/>


### Buff의 적용
![image](https://github.com/user-attachments/assets/54f39d8f-dd2a-4730-8dc0-3b55b14ddcb8)

- 위에서 Buff가 줄 '효과'를 정의했으니, 이제 해당 로직을 호출할 객체가 필요합니다.

- 해당 '효과' 로직을 호출하는 것은 버프의 가해자 측에서 버프에 관련된 정보를 IR4BuffReceiveInterface를 통하여 넘겨주면, 버프를 Receive하는 쪽에서 관리하며 어떻게 적용할지 결정하도록 하였습니다.

- 위에서 결정한 '효과' 로직을 호출할 방식 (간격을 두고 여러번 호출하는지, 한번만 호출하는지, 몇 초동안 유지되는지 등) 을 선택할 수 있도록 FR4BuffSettingDesc를 정의하고 , Buff를 적용받는 객체에서 참고할 수 있도록 하였습니다.
- [[R4BuffStruct.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Buff/R4BuffStruct.h) 

```
/**
 * Buff 사용에 관한 설정 Desc.
 */
USTRUCT ( BlueprintType )
struct RAID4_API FR4BuffSettingDesc
{
	GENERATED_BODY()

	FR4BuffSettingDesc()
		: BuffMode(EBuffMode::Instant)
		, IntervalTime(0.f)
		, BuffDurationType(EBuffDurationType::OneShot)
		, Duration(0.f)
	{ }

	// 버프 적용 방식
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	EBuffMode BuffMode;
	
	// 버프가 간격을 두고 실행이 될때, 간격의 시간을 설정
	UPROPERTY( EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="BuffMode == EBuffMode::Interval && BuffDurationType != EBuffDurationType::OneShot",
			EditConditionHides, ClampMin = "0.01", UIMin = "0.01"))
	float IntervalTime;
	
	// 버프 지속 시간 방식
	UPROPERTY( EditAnywhere, BlueprintReadOnly )
	EBuffDurationType BuffDurationType;
	
	// 버프의 '지속 시간'으로 사용, 버프에 따라 다르게 동작할 수 있으니 확인 후 사용
	UPROPERTY( EditAnywhere, BlueprintReadOnly,
		meta=(EditCondition="BuffDurationType == EBuffDurationType::Duration",
			EditConditionHides, ClampMin = "0.0", UIMin = "0.0"))
	float Duration;
};
```
- BP에서 Buff를 설정 시 Buff의 적용 방식도 설정 할 수 있도록 했습니다.(Skill 관련 부분)
<img src="https://github.com/user-attachments/assets/fe4ffec5-2a9a-46c1-8411-db97dabe5f2d" width="650" height="200"/>

- Character의 경우 Buff의 다양한 적용 방식을 모두 처리할 수 있어야 하며, Character가 적용 받는 Buff의 경우 Server에서 주로 처리하면서도, Client에서 어떠한 Buff가 적용되었는지 알아야 하는 등 Replicate 기능도 필요했기 때문에 BuffManageComponent를 만들어, Buff의 관리를 위임하도록 하였습니다.

- BuffManageComponent는 TickComponent()함수를 통해 시간을 체크하여 여러 적용 방식의 Buff를 처리할 수 있도록 하였으며, 필요에 따라 UR4BuffBase에 등록된 GameplayTags에 맞춰 Buff를 Block할 수 있도록 하였습니다.

- [[UR4BuffManageComponent.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Buff/R4BuffManageComponent.h)
/
[[UR4BuffManageComponent.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Buff/R4BuffManageComponent.cpp)
```
public:    
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:	
	// 버프를 추가
	void AddBuff( AActor* InInstigator, const TSubclassOf<UR4BuffBase>& InBuffClass, const FR4BuffSettingDesc& InBuffSettingDesc );

	// 무시할 버프의 태그를 추가
	void AddBlockingBuffTag( const FGameplayTag& InTag, EGameplayTagQueryType InQueryType );

	// 태그로 해당하는 버프를 모두 제거
	void RemoveBuffAllByTag( const FGameplayTag& InTagToQuery, EGameplayTagQueryType InQueryType );

	// 버프를 업데이트.
	bool _UpdateBuffs( float InNowServerTime );

private:
	// 업데이트가 필요한, 적용된 버프 정보들을 관리. 순서 유지를 보장하지 않음.
	UPROPERTY( Transient, VisibleInstanceOnly, Category = "Buff" )
	TArray<FAppliedBuffInfo> UpdatingBuffs;

	// 무시할 버프가 있다면, 무시할 버프의 태그를 등록. (부모로 사용되어도 일치)
	UPROPERTY( Transient, VisibleInstanceOnly, Category = "Buff" )
	FGameplayTagContainer BlockingBuffTags_Match;

	// 서버에서 적용되어 있는 버프들의 정보, 순서 유지를 보장하지 않음.
	// (서버에서 버프 적용 시 오너에게 정보를 알리는 용도로 사용됨)
	// (정보 확인용으로만 사용할 것)
	UPROPERTY( Replicated, Transient, VisibleInstanceOnly, Category = "Buff" )
	TArray<FServerAppliedBuffInfo> ServerBuffs;
```

## Detect System
![image](https://github.com/user-attachments/assets/b9b63c29-84bc-4967-ae68-c78c2debda98)

- 레벨 내 배치될 수 있는 Actor로, 특정한 상황에 Actor 감지를 위한 Detect System 입니다.

- 인터페이스를 상속 및 구현하여 UShapeComponent를 조합하여 만드는 연속적인 탐지, Trace를 기반으로 일회적인 탐지 등 여러 Detector를 제작할 수 있도록 하였으며, Detector 사용 객체에서는 자세한 클래스의 내용을 몰라도 Interface의 메소드를 호출하는 형태로 Detector를 사용할 수 있도록 하였습니다.

### AR4Detector_Active
- Capsule, Box, Sphere 등 UShapeComponent 기반으로 하는 Component를 통해 Detect를 실행하는 클래스이며, 기본적으로 물리적 계산을 하지 않기 위한 Overlap을 사용합니다.

- BP로 확장하여 여러개의 UShapeComponent를 Attach하여 구성하면, BeginPlay시 해당 Component들을 Find하여 한개의 Delegate로 broadcast할 수 있도록 구성하여 여러 형태의 Detect가 가능하도록 하였습니다.

- 또한 BlueprintImplementableEvent UPROPERTY Specifier를 이용하여 BP에서 Detect 실행 시작 시, Detect 상태 시, Detect 상태 종료 시, Detect 종료 시 확장이 가능하도록 하였고, 이를 통해 여러 Particle을 생성등의 추가적인 처리를 할 수 있도록 하였습니다.

[[R4Detector_Active.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Detect/Detector/R4Detector_Active.cpp)
```
void AR4Detector_Active::BeginPlay()
{
	Super::BeginPlay();

	// Find ShapeComps
	TInlineComponentArray<UShapeComponent*> shapeComps;
	GetComponents<UShapeComponent>( shapeComps, true );
	for ( auto& shapeComp : shapeComps )
	{
		// 멤버로 추가된 Shape Component를 모두 찾아서 한개의 Delegate로 나가도록 연결
		shapeComp->OnComponentBeginOverlap.AddDynamic( this, &AR4Detector_Active::_OnBeginShapeOverlap );
		shapeComp->OnComponentEndOverlap.AddDynamic( this, &AR4Detector_Active::_OnEndShapeOverlap );
	}
}
```
<img src="https://github.com/user-attachments/assets/b277ee55-0e40-4708-ab28-b54a3d5981f0" width="750" height="600"/>

### AR4Detector_Trace
- Trace를 기반으로 Detect를 수행하는 클래스입니다.

- 일회성 또는 간격을 두고 실행하는지 선택하는 ExecutionType, Trace시 사용할 Shape( Capsule, Box, Sphere, Sector )의 정보, BodyInstance 등을 선택할 수 있도록 하여, 해당 데이터를 BP 클래스로 확장하여 지정 후 사용하는 방식으로 여러 형태와 상황의 Trace가 가능하도록 하였습니다.

[[R4Detector_Trace.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Detect/Detector/R4Detector_Trace.h)
```
private:
	// Trace의 실행 타입
	UPROPERTY( EditAnywhere, Category = "Collision" )
	ER4TraceDetectExecutionType ExecutionType;

	// Trace를 체크할 간격시간.
	// 3초 LifeTime에 Interval 1초 일 시, 0, 1, 2 3번 사용됨. 
	UPROPERTY( EditAnywhere, Category="Collision", meta = ( ClampMin = 0.01f, UIMin = 0.01f,
		EditCondition = "ExecutionType == ER4TraceDetectExecutionType::Interval", EditConditionHides) )
	float DetectInterval;

	// Detect에 사용할 모양 정보
	UPROPERTY( EditAnywhere, Category="Collision" )
	FR4TraceDetectShapeInfo ShapeInfo;

	// Detect에 사용할 Collision Response를 위한 BodyInstance
	UPROPERTY( EditAnywhere, Category="Collision", meta=(ShowOnlyInnerProperties, SkipUCSModifiedProperties, AllowPrivateAccess = true) )
	FBodyInstance BodyInstance;
```
<img src="https://github.com/user-attachments/assets/7b49df15-e6df-4ef4-91ab-c787638ccfbe" width="750" height="600"/>

## Skill System
- 전투 시스템을 실질적으로 구현하는 Skill System 입니다.

- Buff System, Detect System, Animation 등 여러가지 Skill에 필요한 기능을 조합하여 Skill을 구성하였습니다.

### UR4SkillBase
![image](https://github.com/user-attachments/assets/a3c3f61c-4a29-4fd0-b1a0-fafdc6351256)

- Skill이 공통적으로 필요로 하는 기능들을 재사용하기 위한 Abstract 클래스입니다.

- 제공하는 기능으로는 Net Flag(Net Flag는 [[네트워크 동기화]](https://github.com/DPRLive/Raid4/blob/master/Readme/NetworkSynchronization.md) 참고)에 맞는 Detector의 생성, Detect 체크, Detect시 시전자 또는 피해자에게 적용하는 'Buff' 지정 및 적용, Skill 사용 시 시전자에게 적용할 'Buff' 지정 및 적용 등이 있습니다.

- 해당 클래스를 상속하여 제공하는 기능을 통해 다양한 형태의 Skill로 확장할 수 있으며, 어떠한 버프를 적용할지, 어떠한 Detector를 사용할지 등의 선택을 BP로 확장 후 데이터를 입력하여 다양한 Skill을 제작할 수 있습니다.

[[R4SkillStruct.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Skill/R4SkillStruct.h)
```
/**
 * Skill에서 진행할 탐지에 관한 정보.
 */
USTRUCT()
struct FR4SkillDetectInfo
{
	GENERATED_BODY()

	FR4SkillDetectInfo()
	: DetectClass( nullptr )
	, DetectorNetFlag( 0 )
	, DetectorOriginCalculator( nullptr )
	, bAttachToMesh( false )
	, MeshSocketName( NAME_None )
	, DetectDesc( FR4DetectDesc() )
	{}
	
	// 어떤 클래스로 Detect를 진행할 것인지?
	// Dummy가 필요한 경우 (bHasVisual) Detect Class를 Replicate 설정해야함! 
	UPROPERTY( EditAnywhere, meta = ( MustImplement = "/Script/Raid4.R4DetectorInterface" ) )
	TSubclassOf<AActor> DetectClass;
	
	// Detector의 Network Spawn 정책을 설정
	// Replicated Detector일 시 Server에서만 Spawn 해야함.
	// particle이 있으나 투사체 등 (시각적인 요소 + 위치 같이) 중요하지 않다면 굳이 Replicate 하지 않고 Local 생성하는걸 권장
	UPROPERTY( EditAnywhere, meta = ( Bitmask, BitmaskEnum = "/Script/Raid4.ER4NetworkFlag" ) )
	uint8 DetectorNetFlag;

	// Detector의 Relative Location의 기준이 될 Origin을 계산하는 Class.
	UPROPERTY( EditAnywhere, meta = ( MustImplement = "/Script/Raid4.R4OriginCalculatorInterface" ) )
	TSubclassOf<UObject> DetectorOriginCalculator;
	
	// Detector를 Mesh에 Attach할 것인지?
	UPROPERTY( EditAnywhere )
	uint8 bAttachToMesh:1;

	// Skeletal Mesh Socket Name
	UPROPERTY( EditAnywhere, meta = ( EditCondition = "bAttachToMesh", EditConditionHides ) )
	FName MeshSocketName;
	
	// 탐지에 관한 Parameter.
	UPROPERTY( EditAnywhere )
	FR4DetectDesc DetectDesc;
};

/**
 * Skill에서 사용할 Buff에 대한 정보.
 */
USTRUCT()
struct FR4SkillBuffInfo
{
	GENERATED_BODY()

	FR4SkillBuffInfo()
	: BuffNetFlag ( 0 )
	, BuffInfo( FR4BuffWrapper() )
	{}
	
	// Buff를 적용할 Network 정책을 설정
	UPROPERTY( EditAnywhere, meta = ( Bitmask, BitmaskEnum = "/Script/Raid4.ER4NetworkFlag" ) )
	uint8 BuffNetFlag;
	
	// 적용할 버프 정보
	UPROPERTY( EditAnywhere )
	FR4BuffWrapper BuffInfo;
};

/**
 * Skill이 Detect시 줄 버프 대한 정보. ( Server 에서만 적용 )
 * 데미지도 버프로 적용 시키면 됨
 */
USTRUCT()
struct FR4SkillDetectBuffInfo
{
	GENERATED_BODY()

	FR4SkillDetectBuffInfo()
	: Target( ETargetType::Victim )
	, BuffInfo( FR4BuffWrapper() )
	{}
	
	// 버프를 적용할 타겟. Instigator : 나. Victim : Detect 된 Actor
	UPROPERTY( EditAnywhere )
	ETargetType Target;
	
	// 적용할 버프 정보
	UPROPERTY( EditAnywhere )
	FR4BuffWrapper BuffInfo;
};
```

<img src="https://github.com/user-attachments/assets/0c2de3dd-bc2e-48b1-ba41-bb949c1da9fd" width="1100" height="550"/>

### UR4AnimSkillBase
- Animation을 사용하는 스킬을 위한 Abstract 클래스입니다. UR4SkillBase를 상속받아 제공받는 기능들을 이용할 수 있으면서, Animation에 관련한 기능도 추가되어 있습니다.

- Animation과 관련한 기능으로는 서버로의 Animation Play 요청 기능, Animation의 Play와 End 시점 처리, Animation Play 후 Lambda를 예약하고 적절한 시간 뒤 실행하는 Execute 기능 등이 있습니다.


#### Execute 기능
- Skill에서 Animation을 사용할 때, 적절한 타이밍에 Detect를 실행하거나 Buff를 적용하는 등의 작업이 필요한 경우가 많습니다. 처음에는 AnimNotify에 Delegate를 설정하여 Animation 실행 시 Notify를 수신하고 특정 작업을 수행하는 구조로 구현했습니다. 그러나 동일한 Anim을 사용하는 경우, Anim을 실행하지 않은 객체에도 알림이 전달되는 문제가 발생했습니다. 이는 AnimMontage와 그 안에 포함된 각 AnimNotify가 하나의 Instance로 유지되는 구조 때문입니다. AnimMontage는 하나의 Instance로 유지되며, AnimInstance에서 MontageInstance를 생성하여 각 객체의 AnimMontage 재생과 상태를 추적하는 방식이었습니다.

- 이 문제를 해결하기 위해 최종적으로 선택한 구조는, 어떤 Animation에 의해 실행되었는지와 Server에서 할당한 Anim Key([[네트워크 동기화]](https://github.com/DPRLive/Raid4/blob/master/Readme/NetworkSynchronization.md) 참고), 실행할 Lambda, 몇 초 뒤에 실행할 것인지에 대한 Delay 정보를 TArray에 등록하고, TickComponent()에서 이를 업데이트하여 적절히 실행하는 방식입니다. 이 구조는 이전 문제를 해결하는 동시에 Anim과의 의존성을 줄일 수 있었습니다.

- 또한 Detect와 Buff 작업은 데이터를 설정해두면 Anim이 실행될 때 자동으로 작업이 예약되도록 하였고, Anim이 종료되면 자동으로 작업 예약이 해제되도록 구현했습니다. 

- 그러나 같은 Anim을 두 번 사용할 경우, Anim의 Blend Out 시점을 End Play Anim으로 판별하더라도 Montage Instance는 파괴되지만, Blend Out은 큐에서 대기한 후 진행되기 때문에 두 번째 실행이 Begin()되고 나서 첫 번째 실행이 End()되는 순서로 로직이 진행됩니다. 이로 인해 두 번째 실행 시 예약한 작업이 즉시 제거되는 문제가 발생했습니다.

- 이 문제를 해결하기 위해 Montage Instance의 OnMontageInstanceStopped를 오버라이드하고, delegate로 broadcast하여 파괴 시점을 수신하는 방식으로 조치하였습니다.

[[R4AnimSkillBase.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Skill/R4AnimSkillBase.cpp)
```
/**
 *  Skill Anim Key에 맞는, 특정 시간 뒤에 Execute 예약 추가.
 *  멤버로 등록된 Skill Anim Key만 추가 가능.
 *  InFunc에 Update 배열에 무언가를 추가하는 로직 작성 시 추가 로직 반복에 의한 메모리 누수 주의.
 *  @param InSkillAnimKey : 멤버로 등록된, Play할 Skill Anim Info
 *  @param InFunc : 실행시킬 Lambda, R-value, this 캡쳐 시 weak 캡쳐할 것
 *  @param InDelay : 실행 Delay. ( Delay <= 0 이면, 바로 실행 됨 ) 
 *  @param InDelayRate : Delay를 얼마나 빠르게 체크할지, ( 실제로 InDelay / InDelayRate 뒤에 실행 됨 ). InDelayRate < 0.f인 경우는 처리하지 않음 !
 */
void UR4AnimSkillBase::AddExecute( int32 InSkillAnimKey, TFunction<void()>&& InFunc, float InDelay, float InDelayRate )
{
	if ( !IsValidSkillAnimKey(InSkillAnimKey) )
	{
		LOG_WARN( R4Skill, TEXT("Skill Anim Key [%d] Is Invalid. Check SkillAnim Replicate State."), InSkillAnimKey );
		return;
	}

	if( !InFunc )
	{
		LOG_WARN( R4Skill, TEXT("InFunc is invalid.") );
		return;
	}

	// InPlayRate가 음수이면, 처리하지 않음.
	if( InDelayRate < 0.f )
	{
		LOG_WARN( R4Skill, TEXT("Negative InDelayRate [%f] is not processed."), InDelayRate );
		return;
	}

	// division by zero 방지
	InDelayRate = FMath::Max( KINDA_SMALL_NUMBER, InDelayRate );
	
	// 실제로 적용할 delay 계산
	InDelay /= InDelayRate;

	// Delay가 너무 작으면, 그냥 바로 실행
	if ( InDelay <= KINDA_SMALL_NUMBER )
	{
		if( InFunc )
			InFunc();

		return;
	}
	
	// 아니면 Update 대기열에 넣고 Tick On
	PendingExecutes.Emplace
	(
		FR4AnimSkillExecuteInfo
		(
			InSkillAnimKey,
			_GetNowMontageInstanceID( InSkillAnimKey ),
			R4GetServerTimeSeconds( GetWorld() ) + InDelay,
			MoveTemp(InFunc)
		)
	);
	
	SetComponentTickEnabled( true );
}

/**
 *  Detect Execute Net Flag에 맞춰 추가
 *  @param InSkillAnimKey : Execute가 필요한 Skill Anim Key
 *  @param InTimeDetectInfo : Delay와 Detect 정보
 *  @param InDelayRate :Delay를 얼마나 빠르게 체크할지, ( 실제로 InDelay / InDelayRate 뒤에 실행 됨 ). InDelayRate < 0.f인 경우는 처리하지 않음 !
 */
void UR4AnimSkillBase::AddDetectExecute( int32 InSkillAnimKey, const FR4SkillTimeDetectWrapper& InTimeDetectInfo, float InDelayRate )
{
	// InPlayRate가 음수이면, 처리하지 않음.
	if( InDelayRate < 0.f )
	{
		LOG_WARN( R4Skill, TEXT("Negative InDelayRate [%f] is not processed."), InDelayRate );
		return;
	}

	// Replicated Detector일 시 Server에서만 Spawn 하는지 확인
	{
		// Parse Net Flag
		bool bReplicate = InTimeDetectInfo.DetectEffect.DetectorInfo.DetectClass.GetDefaultObject()->GetIsReplicated();
		bool bServerSpawn = ( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Server) );
		bool bLocalSpawn = ( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Local ) );
		bool bSimulatedSpawn = ( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag & static_cast<uint8>( ER4NetworkFlag::Simulated ) );
		
		if ( !ensureMsgf( bReplicate ? ( bServerSpawn && !bLocalSpawn && !bSimulatedSpawn ) : true,
				TEXT("Replicated Detector must be spawned only on Server") ) )
			return;
	}
	
	// Net Flag가 일치하지 않으면 추가하지 않음.
	if ( !IsMatchNetFlag( InTimeDetectInfo.DetectEffect.DetectorInfo.DetectorNetFlag ) )
		return;
	
	AddExecute( InSkillAnimKey, [thisPtr = TWeakObjectPtr<UR4AnimSkillBase>(this), &detectInfo = InTimeDetectInfo.DetectEffect]()
	{
		if ( thisPtr.IsValid() )
			thisPtr->ExecuteDetect( detectInfo );
	}, InTimeDetectInfo.DelayTime, InDelayRate );
}

/**
 *  buff Execute Net Flag에 맞춰 추가
 *  @param InSkillAnimKey : Execute가 필요한 Skill Anim Key
 *  @param InTimeBuffInfo : buff 정보
 *  @param InDelayRate :Delay를 얼마나 빠르게 체크할지, ( 실제로 InDelay / InDelayRate 뒤에 실행 됨 ). InDelayRate < 0.f인 경우는 처리하지 않음 !
 */
void UR4AnimSkillBase::AddBuffExecute( int32 InSkillAnimKey, const FR4SkillTimeBuffWrapper& InTimeBuffInfo, float InDelayRate )
{
	// InPlayRate가 음수이면, 처리하지 않음.
	if( InDelayRate < 0.f )
	{
		LOG_WARN( R4Skill, TEXT("Negative InDelayRate [%f] is not processed."), InDelayRate );
		return;
	}

	// Net Flag가 일치하지 않으면 추가하지 않음.
	if ( !IsMatchNetFlag( InTimeBuffInfo.SkillBuffInfo.BuffNetFlag ) )
		return;
	
	AddExecute( InSkillAnimKey, [thisPtr = TWeakObjectPtr<UR4AnimSkillBase>(this), &InTimeBuffInfo]()
	{
		if ( thisPtr.IsValid() )
		{
			// 나에게 버프 적용
			thisPtr->ApplySkillBuff( InTimeBuffInfo.SkillBuffInfo );
		}
	}, InTimeBuffInfo.DelayTime, InDelayRate );
}

```

- Execute는 추가될 때 TArray에 저장되며, TickComponent()에서 업데이트를 처리합니다. 이 과정에서 TArray를 순회하며, 시간이 경과했는지 확인한 후 등록된 Lambda를 실행합니다. 그러나 순회가 끝나기 전에 Lambda에서 TArray의 원소를 삭제하는 코드가 존재한다면, TArray의 인덱스가 변경되어 예기치 못한 문제가 발생할 수 있습니다. 이를 방지하기 위해 TArray를 private으로 설정하고, 별도의 Remove 함수를 작성하지 않았습니다. Montage Instance ID를 통해 현재 실행하고 있는 Anim이 등록될 때 예약된 작업인지 확인하여 실행 여부를 판단하고, 조건이 맞으면 실행, 아니면 자동으로 삭제되도록 구현했습니다.

[[R4AnimSkillBase.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Skill/R4AnimSkillBase.cpp)
```
/**
 *  실행 대기중인 Executes를 Update.
 *  @param InNowServerTime : 현재 서버 시간
 */
void UR4AnimSkillBase::_UpdateExecute( float InNowServerTime )
{
	for( auto it = PendingExecutes.CreateIterator(); it; ++it )
	{
		// 현재 Skill Anim montage의 Active Montage Instance ID와 일치하지 않으면, 제거
		if ( it->MontageInstanceID != _GetNowMontageInstanceID( it->SkillAnimKey ) )
		{
			it.RemoveCurrentSwap();
			continue;
		}
		
		// Update 가능 상태이고, 시간이 지났다면, execute 후 제거
		if ( InNowServerTime > it->ExecuteServerTime )
		{
			if ( it->Func )
				it->Func();
			
			it.RemoveCurrentSwap();
		}
	}
}
```
	

- 위에서 구현된 BuffExecute, DetectExecute는 역시 AnimSkillBase를 BP로 확장하여 데이터 입력으로 손쉽게 추가할 수 있습니다.

[[R4SkillStruct.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Skill/R4SkillStruct.h)
```
/**
 * Skill에서 특정 시간에 Detect시 사용
 */
USTRUCT()
struct FR4SkillTimeDetectWrapper
{
	GENERATED_BODY()

	FR4SkillTimeDetectWrapper()
	: DelayTime( 0.f )
	, DetectEffect( FR4SkillDetectBuffWrapper() )
	{}

	// Delay Time, ( AnimMontage의 특정 Section Play된 후 시간 ) 
	UPROPERTY( EditAnywhere )
	float DelayTime;

	// 해당 시점에 할 Detect와 적용할 Buff
	UPROPERTY( EditAnywhere )
	FR4SkillDetectBuffWrapper DetectEffect;
};

/**
 * Skill에서 특정 시간에 Apply Buff시 사용
 */
USTRUCT()
struct FR4SkillTimeBuffWrapper
{
	GENERATED_BODY()

	FR4SkillTimeBuffWrapper()
	: DelayTime( 0.f )
	, SkillBuffInfo( FR4SkillBuffInfo() )
	{}
	
	// Delay Time
	UPROPERTY( EditAnywhere )
	float DelayTime;
	
	// 적용할 버프 정보
	UPROPERTY( EditAnywhere )
	FR4SkillBuffInfo SkillBuffInfo;
};

```

<img src="https://github.com/user-attachments/assets/da6fe61b-7f42-4043-8cce-bfb830591f93" width="1100" height="900"/>
