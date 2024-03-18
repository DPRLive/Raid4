#pragma once

/**
 * Stat을 사용 시 Access를 위한 함수들을 매크로로 편하게 정의
 */

#define R4STAT_BASE_VALUE_GETTER( Name ) \
    float GetBase##Name() const { return Name.GetBaseValue(); }

#define R4STAT_BASE_VALUE_SETTER( Name ) \
    void SetBase##Name(float InBase##Name) { Name.SetBaseValue( InBase##Name ); }

#define R4STAT_BASE_VALUE_DELEGATE( Name ) \
    FOnChangeStatData& GetOnChangeBase##Name() { return Name.OnChangeBaseValue; }

#define R4STAT_MODIFIER_VALUE_GETTER( Name ) \
    float GetModifier##Name() const { return Name.GetModifierValue(); }

#define R4STAT_MODIFIER_VALUE_SETTER( Name ) \
    void SetModifier##Name(float InModifier##Name) { Name.SetModifierValue( InModifier##Name ); }

#define R4STAT_MODIFIER_VALUE_DELEGATE( Name ) \
    FOnChangeStatData& GetOnChangeModifier##Name() { return Name.OnChangeModifierValue; }

#define R4STAT_CURRENT_VALUE_GETTER( Name ) \
    float GetCurrent##Name() const { return Name.GetCurrentValue(); }

#define R4STAT_CURRENT_VALUE_SETTER( Name ) \
    void SetCurrent##Name(float InCurrent##Name) { Name.SetCurrentValue( InCurrent##Name ); }

#define R4STAT_CURRENT_VALUE_DELEGATE( Name ) \
    FOnChangeStatData& GetOnChangeCurrent##Name() { return Name.OnChangeCurrentValue; }

#define R4STAT_STAT_ACCESSORS( Name )       \
    R4STAT_BASE_VALUE_GETTER( Name )        \
    R4STAT_BASE_VALUE_SETTER( Name )        \
    R4STAT_BASE_VALUE_DELEGATE( Name )      \
    R4STAT_MODIFIER_VALUE_GETTER( Name )    \
    R4STAT_MODIFIER_VALUE_SETTER( Name )    \
    R4STAT_MODIFIER_VALUE_DELEGATE( Name )  \

#define R4STAT_CONSUMABLE_STAT_ACCESSORS( Name )   \
    R4STAT_STAT_ACCESSORS( Name )                  \
    R4STAT_CURRENT_VALUE_GETTER( Name )            \
    R4STAT_CURRENT_VALUE_SETTER( Name )            \
    R4STAT_CURRENT_VALUE_DELEGATE( Name )