﻿#pragma once

/**
 * Stat을 사용 시 Access를 위한 함수들을 매크로로 편하게 정의
 */

#define R4STAT_BASE_VALUE_GETTER( Name ) \
    float GetBase##Name() const { return Name.GetBaseValue(); }

#define R4STAT_BASE_VALUE_SETTER( Name ) \
    void SetBase##Name(float InBase##Name) { Name.SetBaseValue( InBase##Name ); }

#define R4STAT_MODIFIER_VALUE_GETTER( Name ) \
    float GetModifier##Name() const { return Name.GetModifierValue(); }

#define R4STAT_MODIFIER_VALUE_SETTER( Name ) \
    void SetModifier##Name(float InModifier##Name) { Name.SetModifierValue( InModifier##Name ); }

#define R4STAT_CURRENT_VALUE_GETTER( Name ) \
    float GetCurrent##Name() const { return Name.GetCurrentValue(); }

#define R4STAT_CURRENT_VALUE_SETTER( Name ) \
    void SetCurrent##Name(float InCurrent##Name) { Name.SetCurrentValue( InCurrent##Name ); }

#define R4STAT_STAT_DELEGATE( Name ) \
    FOnChangeStatData& GetOnChange##Name() { return Name.OnChangeStatData; }

#define R4STAT_CURRENT_VALUE_DELEGATE( Name ) \
    FOnChangeCurrentStatData& GetOnChangeCurrent##Name() { return Name.OnChangeCurrentValue; }

#define R4STAT_STAT_INITTER( Name ) \
    void Init##Name(float InBaseValue) { Name.InitStatData(InBaseValue); }

#define R4STAT_STAT_ACCESSORS( Name )       \
    R4STAT_STAT_INITTER( Name )             \
    R4STAT_BASE_VALUE_GETTER( Name )        \
    R4STAT_BASE_VALUE_SETTER( Name )        \
    R4STAT_MODIFIER_VALUE_GETTER( Name )    \
    R4STAT_MODIFIER_VALUE_SETTER( Name )    \
    R4STAT_STAT_DELEGATE( Name )

#define R4STAT_CONSUMABLE_STAT_ACCESSORS( Name )   \
    R4STAT_STAT_ACCESSORS( Name )                  \
    R4STAT_CURRENT_VALUE_GETTER( Name )            \
    R4STAT_CURRENT_VALUE_SETTER( Name )            \
    R4STAT_CURRENT_VALUE_DELEGATE( Name )          

/**
 * OnRep 시 바뀐 데이터를 Broadcast하기 위한 Macro.
 * Shadow data와 비교해서 바뀐것만 broadcast
 */
#define R4STAT_STAT_OnRep( Now, Prev ) \
    if(!FMath::IsNearlyEqual(Now.GetBaseValue(), Prev.GetBaseValue()) || !FMath::IsNearlyEqual(Now.GetModifierValue(), Prev.GetModifierValue())) \
    {                                                                                                                                            \
        if(Now.OnChangeStatData.IsBound())                                                                                                       \
            Now.OnChangeStatData.Broadcast(Now.GetBaseValue(), Now.GetModifierValue());                                                          \
    }

#define R4STAT_CONSUMABLE_STAT_OnRep( Now, Prev ) \
    R4STAT_STAT_OnRep( Now, Prev )                                              \
    if(!FMath::IsNearlyEqual(Now.GetCurrentValue(), Prev.GetCurrentValue()))    \
    {                                                                           \
        if(Now.OnChangeCurrentValue.IsBound())                                  \
            Now.OnChangeCurrentValue.Broadcast(Now.GetCurrentValue());          \
    }