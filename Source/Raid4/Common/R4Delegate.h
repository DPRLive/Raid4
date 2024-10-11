#pragma once

#include <Delegates/Delegate.h>

struct FAnimMontageInstance;

/**
 *  여러 곳에서 사용할 Delegate 모음
 *  Dynamic은 안돼요.
 */

// Montage Instance가 Clear되는 타이밍에 반환.
DECLARE_MULTICAST_DELEGATE_OneParam( FOnClearMontageInstance, FAnimMontageInstance& );