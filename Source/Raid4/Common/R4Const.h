#pragma once

/**
 * 전역적으로 사용할 const, constexpr 모음
 */

// Object Pool의 기본 최대 저장 크기
constexpr static uint16 GObjectPoolSize = 100;

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

namespace Damage
{
	// 방어 상수
	constexpr static float G_DefenseConstant = 100.f;

	// 치명타 시 데미지 증가량
	constexpr static float G_CriticalFactor = 1.3f;

	// 데미지의 랜덤 증감량 아랫 경계값
	constexpr static float G_RandomFactorLower = 0.9f;

	// 데미지의 랜덤 증감량 윗 경계값
	constexpr static float G_RandomFactorUpper = 1.1f;
}
