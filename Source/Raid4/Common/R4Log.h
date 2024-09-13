#pragma once

/**
 * 로그 카테고리 추가. (R4Log.cpp 에도 카테고리를 추가해야함)
 */
DECLARE_LOG_CATEGORY_EXTERN( R4Data, Log, All );
DECLARE_LOG_CATEGORY_EXTERN( R4Input, Log, All );
DECLARE_LOG_CATEGORY_EXTERN( R4Effect, Log, All );
DECLARE_LOG_CATEGORY_EXTERN( R4Log, Log, All );
DECLARE_LOG_CATEGORY_EXTERN( R4Stat, Log, All );
DECLARE_LOG_CATEGORY_EXTERN( R4Skill, Log, All );

/**
 * 로그 매크로 모음. SHIPPING 시에 지워지지 않으면 패키징 안되므로 #if 추가
 */
#if UE_BUILD_SHIPPING
    LOG_N( category, text, ... ) {}
	LOG_WARN( category, text, ... ) {}
	LOG_ERROR( category, text, ... ) {}
	LOG_SCREEN( category, Color, text, ... ) {}
#else
    /**
     * 출력 로그 (log) 용 로그. 
     */
    #define LOG_N( category, text, ... )										\
    {																			\
        FString msg = FString::Printf( TEXT("%s : "), __FUNCTION__ TEXT("()"))	\
        + FString::Printf(text, ##__VA_ARGS__);								    \
        UE_LOG(category, Log, TEXT("%s"), *msg);								\
    }

    /**
     * 출력 로그 (Warning Category) 용 로그
     */
    #define LOG_WARN( category, text, ... )										\
    {																		    \
        FString msg = FString::Printf( TEXT("%s : "), __FUNCTION__ TEXT("()"))	\
        + FString::Printf(text, ##__VA_ARGS__);								    \
        UE_LOG(category, Warning, TEXT("%s"), *msg);							\
    }

    /**
     * 출력 로그 (Error Category) 용 로그
     */
    #define LOG_ERROR( category, text, ... )									\
    {																			\
    	FString msg = FString::Printf( TEXT("%s : "), __FUNCTION__ TEXT("()"))	\
    	+ FString::Printf(text, ##__VA_ARGS__);									\
    	UE_LOG(category, Error, TEXT("%s"), *msg);								\
    }

    /**
     * 출력로그 + 에디터 스크린 동시에 로그
     */
    #define LOG_SCREEN( Color, text, ... )									   \
    {																		   \
    	LOG_WARN( LogTemp, text, ##__VA_ARGS__);							   \
    	FString msg = FString::Printf( TEXT("%s : "), __FUNCTION__ TEXT("()")) \
    	+ FString::Printf(text, ##__VA_ARGS__);								   \
    	GEngine->AddOnScreenDebugMessage( -1, 2.0f, Color, msg );			   \
    }
#endif
