#pragma once

#include "SingletonBase.h"

/**
 * static 멤버를 전역 범위에서 초기화
 */
template <typename Type>
Type* TSingletonBase<Type>::Instance = nullptr;

/**
 * 소멸자
 */
template <typename Type>
TSingletonBase<Type>::~TSingletonBase()
{
	Instance = nullptr;
}

/**
 * 인스턴스 생성 함수
 */
template <typename Type>
Type* TSingletonBase<Type>::Create()
{
    if (Instance == nullptr)
        Instance = new Type();

    return Instance;
}

/**
 * 인스턴스 반환 함수
 */
template <typename Type>
Type* TSingletonBase<Type>::Get()
{
    return Instance;
}

/**
 * 인스턴스 해제 함수
 */
template <typename Type>
void TSingletonBase<Type>::Destroy()
{
    if (Instance != nullptr)
        delete Instance;
}

/**
 * 유효성 검사함수
 */
template <typename Type>
bool TSingletonBase<Type>::IsValid()
{
    return Instance != nullptr;
}
