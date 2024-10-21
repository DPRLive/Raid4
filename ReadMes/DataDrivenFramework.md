# 데이터 기반 멀티플레이어 전투 시스템 프레임워크
- 스킬이나 버프 등 다양한 전투 관련 로직을 구성할 때, C++ 클래스의 사용을 최소화하고, 데이터의 조정과 구성만으로 다양한 형태로 동작할 수 있는 구조가 필요함을 느꼈습니다.

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

- 해당 효과 로직을 호출하는 것은 버프의 가해자 측에서 버프에 관련된 정보를 IR4BuffReceiveInterface를 통하여 넘겨주면, 버프를 Receive하는 쪽에서 관리하며 어떻게 적용할지 결정하도록 하였습니다.

- 위에서 결정한 '효과' 로직을 실제로 어떤 방식 호출할 방식을 선택할 수 있도록 FR4BuffSettingDesc를 정의하고 , Buff를 적용받는 객체에서 참고할 수 있도록 하였습니다.
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

------------


