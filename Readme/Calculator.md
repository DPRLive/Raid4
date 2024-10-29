
# Calculator
- 특정 상황에서 'Value'를 산출할 때 다양한 방식으로 'Value'를 구하게 도와주는 Strategy 클래스입니다.

- 일반적으로 Calculator는 아래처럼 사용됩니다.
    * 'Value' 산출이 필요한 상황에 맞는 Interface를 상속하여 구현하며, 원하는 'Value' 산출 로직을 작성한 Calculator Class를 구현합니다.
    * 'Value' 산출이 필요한 상황에 TSubclassOf 변수를 두고, Interface 제한을 설정한 뒤 BP Editor에서 다양한 방식으로 구현한 Calculator 중 필요한 로직의 Calculator를 지정합니다.
    * 지정된 Class는 CDO와 Interface를 통해 런타임에 활용되며 작성된 로직대로 'Value'를 산출합니다.

- 성능 향상을 위해 CDO를 적극 활용하였으며, CDO를 활용하기 때문에 런타임에 특별한 상태값이 필요 없는 단순 계산 용도로만 사용합니다.
------------
## 목차
- ### [1. R4InteractValueCalculator](#r4interactvaluecalculator)
    * [R4InteractValueCalculator_StatBase](#r4interactvaluecalculator_statbase)
    
- ### [2. R4OriginCalculatorInterface](#OriginCalculatorInterface)
    * [AR4Detector_Active](#ar4detector_active)
    * [AR4Detector_Trace](#ar4detector_trace)

------------
## R4InteractValueCalculator
- 두 객체 사이 상호작용이 발생할 때 활용할 수 있는 Calculator입니다.

- 피해자와 가해자가 정해지면, 구현된 로직을 따라 필요한 'Value'를 산출합니다.

![image](https://github.com/user-attachments/assets/11dcd236-18a9-4ff0-a337-42ebb480b755)

### R4InteractValueCalculator_StatBase
- InteractValueCalculator 중 하나인 StatBase Calculator입니다.

- 피해자 / 가해자 중 하나를 선택하고, 어떤 스탯을 가져올지 설정하면, CalculateInteractValue() 시 설정된 값대로 'Value'를 산출합니다.

[[UR4InteractValueCalculator_StatBase.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Calculator/Interaction/R4InteractValueCalculator_StatBase.h)

```
		/**
	*  Stat에 기반한 Interact Value를 계산.
	*  @param InInstigator : 가해자 객체
	*  @param InVictim : 피해자 객체
	*/
	virtual float CalculateInteractValue( const AActor* InInstigator, const AActor* InVictim ) const override;

private:
	// 상대의 스탯인지 나의 스탯인지
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	ETargetType TargetStatOwner;
	
	// 기반이 되는 스탯
	UPROPERTY( EditDefaultsOnly, meta = (Categories = "Stat", AllowPrivateAccess = true))
	FGameplayTag StatTag;

	// 스탯 피연산자 타입
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	EStatOperandType OperandType;

	// 비율 ( 해당 Stat 값에 곱해져서 Value가 구해짐 )
	UPROPERTY( EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float Proportion;
```

[[R4InteractValueCalculator_StatBase.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Calculator/Interaction/R4InteractValueCalculator_StatBase.cpp)

```
/**
*  Stat에 기반한 Value를 계산.
*  @param InInstigator : 가해자 객체
*  @param InVictim : 피해자 객체
*  @return : Stat에 기반하여 계산 된 Value
*/
float UR4InteractValueCalculator_StatBase::CalculateInteractValue(const AActor* InInstigator, const AActor* InVictim) const
{
	const AActor* target = (TargetStatOwner == ETargetType::Instigator ? InInstigator : InVictim);
	if(!IsValid(target))
		return 0.f;
	
	if (const IR4TagStatQueryInterface* queryTarget = Cast<IR4TagStatQueryInterface>(target))
	{
		switch (OperandType)
		{
		case EStatOperandType::Base:
			if (FR4StatInfo* stat = queryTarget->GetStatByTag(StatTag))
				return stat->GetBaseValue() * Proportion;
			break;

		case EStatOperandType::Current:
			if (FR4CurrentStatInfo* stat = queryTarget->GetCurrentStatByTag(StatTag))
				return stat->GetCurrentValue() * Proportion;
			break;

		default: case EStatOperandType::Total:
			if (FR4StatInfo* stat = queryTarget->GetStatByTag(StatTag))
				return stat->GetTotalValue() * Proportion;
			break;
		}
	}
	
	return 0.f;
}
```

- 아래는 실제 사용된 예시로, Damage Buff에서 사용되었으며, 적용 시 'AttackPower' Stat 기반으로 Damage를 산출한 예시입니다.

<img src="https://github.com/user-attachments/assets/c6378a2e-7cba-4fb0-9655-32cd816df138" width="450" height="380"/>

<img src="https://github.com/user-attachments/assets/33bbc6e5-4461-4f27-be1d-439de6613f54" width="450" height="380"/>

------------
## R4OriginCalculatorInterface
- 특정 객체를 기준으로, Transform을 구할 때 사용하는 Calculator입니다.

- 기준이 정해지면, 구현된 로직을 따라 필요한 'Value (Transform)'를 산출합니다.

![image](https://github.com/user-attachments/assets/17c96f27-092a-49bb-ba63-91e513d76552)

### R4OriginCalculator_MeshSocket
- OriginCalculator 중 하나인 MeshSocket Calculator입니다.

- 구하고 싶은 MeshSocket의 이름을 지정하고, CalculateOrigin() 시 설정된 값대로 'Value (Transform)'를 산출합니다.

[[UR4OriginCalculator_MeshSocket.h]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Calculator/Origin/R4OriginCalculator_MeshSocket.h)

```
private:
	// 계산할 Mesh Socket의 Name.
	UPROPERTY( EditAnywhere )
	FName SocketName;
```

[[R4OriginCalculator_MeshSocket.cpp]](https://github.com/DPRLive/Raid4/blob/master/Source/Raid4/Calculator/Origin/R4OriginCalculator_MeshSocket.cpp)

```
/**
 *  Mesh의 Socket을 기준으로 Origin으로 계산.
 *  CDO를 사용!
 *  @param InRequestObj : 계산을 요청한 Object
 *  @param InActor : 계산의 기준이 되는 Actor.
 */
FTransform UR4OriginCalculator_MeshSocket::CalculateOrigin( const UObject* InRequestObj, const AActor* InActor ) const
{
	const ACharacter* owner = Cast<ACharacter>( InActor );
	if( !IsValid( owner ) )
	{
		LOG_WARN( R4Log, TEXT("UR4OriginCalculator_MeshSocket requires 'ACharacter' based class") );
		return FTransform::Identity;
	}

	const USkeletalMeshComponent* skelMesh = owner->GetMesh();
	if( !IsValid( skelMesh ) )
	{
		LOG_WARN( R4Log, TEXT("USkeletalMeshComponent is invalid.") );
		return FTransform::Identity;
	}

	if ( !skelMesh->DoesSocketExist( SocketName ) )
	{
		LOG_WARN( R4Log, TEXT("SocketName is invalid.") );
		return FTransform::Identity;
	}

	return skelMesh->GetSocketTransform( SocketName );
}
```

- 아래는 실제 사용된 예시로, SkillBase의 Detector Spawn시 사용되었으며, 적용 시 현재 사용되고 있는 SkelMesh의 'SocketName' 기반으로 'Value (Transform)'를 산출합니다.

<img src="https://github.com/user-attachments/assets/8a399bbb-5351-42f0-bd73-cb8e06e3e27b" width="300" height="200"/>

<img src="https://github.com/user-attachments/assets/63f434ca-9405-4b0b-be28-1526b1d14eb5" width="700" height="700"/>