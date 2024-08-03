// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "PoolableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPoolableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Object Pool에 의해 Pooling 될 수 있는 Object에 사용.
 */
class RAID4_API IPoolableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// Pool Object를 Pool에서 사용하기 위한 꺼낸 후 로직 처리
	virtual void PostInitPoolObject() PURE_VIRTUAL(IPoolableInterface::PostInitPoolObject, );

	// Pool Object를 Pool에서 반납하기 위한 반납 전 로직 처리
	virtual void PreReturnPoolObject() PURE_VIRTUAL(IPoolableInterface::PreReturnPoolObject, );
};
