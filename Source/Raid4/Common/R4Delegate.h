#pragma once

#include <Delegates/Delegate.h>

/**
 *  여러 곳에서 사용할 Delegate 모음
 *  Dynamic은 안돼요.
 */

// 오버랩 결과를 넘겨주는 Delegate
DECLARE_MULTICAST_DELEGATE_OneParam( FR4OverlapDelegate, const FOverlapResult& );
