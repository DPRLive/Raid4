#pragma once

/**
 * 전역적으로 사용할 const, constexpr 모음
 */

// Object Pool의 기본 최대 저장 크기
constexpr static uint16 G_ObjectPoolSize = 100;

namespace DTConst
{
	// DT의 유효하지 않은 기본키
	constexpr static int32 G_InvalidPK = 0;
}

namespace Validation
{
	// 서버 <-> 클라이언트 시간 오차 허용 범위
	constexpr static float G_AcceptMinTime = 0.1f;
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

namespace Buff
{
	// Buff 시간 갱신을 얼만큼의 interval 마다 할 것인가?
	constexpr static float G_BuffTickInterval = 0.01f;
}

namespace Collision
{
	// NoCollision Profile
	const static FName G_ProfileNoCollision = TEXT("NoCollision");
}