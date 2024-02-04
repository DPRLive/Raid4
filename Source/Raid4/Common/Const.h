#pragma once

/**
 * 전역적으로 사용할 const, constexpr 모음
 */

namespace DTConst
{
	// DT의 유효하지 않은 기본키
	constexpr static int32 G_InvalidPK = 0;
}

namespace Validation
{
	// 서버에서 쿨타임 검증 시 허용 범위
	constexpr static float G_AcceptMinCoolTime = 0.1f;
}
