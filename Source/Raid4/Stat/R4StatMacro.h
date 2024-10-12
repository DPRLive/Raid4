#pragma once

/**
 * Stat을 사용 시 Access를 위한 함수들을 매크로로 편하게 정의
 */

#define R4STAT_BASE_VALUE_GETTER( Name ) \
    float GetBase##Name() const { return Name.GetBaseValue(); }

#define R4STAT_BASE_VALUE_SETTER( Name ) \
    void SetBase##Name(float InBase##Name) { Name.SetBaseValue( InBase##Name ); }

#define R4STAT_ADD_MODIFIER_VALUE_GETTER( Name ) \
    float GetAddModifier##Name() const { return Name.GetAddModifierValue(); }

#define R4STAT_MULTIPLY_MODIFIER_VALUE_GETTER( Name ) \
    float GetMultiplyModifier##Name() const { return Name.GetMultiplyModifierValue(); }

#define R4STAT_TOTAL_VALUE_GETTER( Name ) \
    float GetTotal##Name() const { return Name.GetTotalValue(); }

#define R4STAT_ADD_MODIFIER_VALUE_SETTER( Name ) \
    void SetAddModifier##Name(float InAddModifier##Name) { Name.SetAddModifierValue( InAddModifier##Name ); }

#define R4STAT_MULTIPLY_MODIFIER_VALUE_SETTER( Name ) \
    void SetMultiplyModifier##Name(float InMultiplyModifier##Name) { Name.SetMultiplyModifierValue( InMultiplyModifier##Name ); }

#define R4STAT_CURRENT_VALUE_GETTER( Name ) \
    float GetCurrent##Name() const { return Name.GetCurrentValue(); }

#define R4STAT_CURRENT_VALUE_SETTER( Name ) \
    void SetCurrent##Name(float InCurrent##Name) { Name.SetCurrentValue( InCurrent##Name ); }

#define R4STAT_STAT_DELEGATE( Name ) \
    FOnChangeStatDataDelegate& OnChange##Name() { return Name.OnChangeStatDataDelegate; }

#define R4STAT_CURRENT_VALUE_DELEGATE( Name ) \
    FOnChangeStatDataDelegate& OnChangeCurrent##Name() { return Name.OnChangeCurrentValueDelegate; }

#define R4STAT_STAT_INITTER( Name ) \
    void Init##Name() { Name.InitStatData(); }

#define R4STAT_STAT_ACCESSORS( Name )               \
    R4STAT_STAT_INITTER( Name )                     \
    R4STAT_BASE_VALUE_GETTER( Name )                \
    R4STAT_BASE_VALUE_SETTER( Name )                \
    R4STAT_ADD_MODIFIER_VALUE_GETTER( Name )        \
    R4STAT_ADD_MODIFIER_VALUE_SETTER( Name )        \
    R4STAT_MULTIPLY_MODIFIER_VALUE_GETTER( Name )   \
    R4STAT_MULTIPLY_MODIFIER_VALUE_SETTER( Name )   \
    R4STAT_TOTAL_VALUE_GETTER( Name )               \
    R4STAT_STAT_DELEGATE( Name )

#define R4STAT_CURRENT_STAT_ACCESSORS( Name )      \
    R4STAT_STAT_ACCESSORS( Name )                  \
    R4STAT_CURRENT_VALUE_GETTER( Name )            \
    R4STAT_CURRENT_VALUE_SETTER( Name )            \
    R4STAT_CURRENT_VALUE_DELEGATE( Name )          

/**
 * OnRep 시 바뀐 데이터를 Broadcast하기 위한 Macro.
 * Shadow data와 비교해서 바뀐것만 broadcast
 */
#define R4STAT_STAT_OnRep( Now, Prev ) \
    if(!FMath::IsNearlyEqual(Now.GetTotalValue(), Prev.GetTotalValue()) )                           \
    {                                                                                               \
        if(Now.OnChangeStatDataDelegate.IsBound())                                                  \
            Now.OnChangeStatDataDelegate.Broadcast(Prev.GetTotalValue(), Now.GetTotalValue());      \
    }

#define R4STAT_CURRENT_STAT_OnRep( Now, Prev ) \
    R4STAT_STAT_OnRep( Now, Prev )                                                                    \
    if(!FMath::IsNearlyEqual(Now.GetCurrentValue(), Prev.GetCurrentValue()))                          \
    {                                                                                                 \
        if(Now.OnChangeCurrentValueDelegate.IsBound())                                                \
            Now.OnChangeCurrentValueDelegate.Broadcast(Prev.GetTotalValue(), Now.GetCurrentValue());  \
    }