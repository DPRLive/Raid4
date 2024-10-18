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

	// 거리 계산 Validation Check 시 오차 허용 범위
	constexpr static float G_AcceptMinLength = 100.f;
}

namespace Damage
{
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

	// Enemy Collision Profile
	const static FName G_ProfileEnemy = TEXT("Enemy");

	// Player Collision Profile
	const static FName G_ProfilePlayer = TEXT("Player");
}

namespace Stat
{
	// 방어 상수
	constexpr static float G_ArmorConstant = 100.f;

	// 쿨타임 감소 상한값
	constexpr static float G_CoolDownReductionUpper = 50.f;
}

namespace Skill
{
	// skill anim 구분 시 사용하는 Key의 Invalid한 값.
	constexpr static int32 G_InvalidSkillAnimKey = INDEX_NONE;

	// Point 선택시 Trace할 길이 (넉넉하게)
	constexpr static float G_PointSkillTraceLength = 20000.f;

	// Point 선택시 Profile
	constexpr static uint8 G_PointSkillTraceChannel = ECC_GameTraceChannel5;
}

namespace Socket
{
	// Health Bar 부착 시 Socket
	const static FName G_HealthBarSocket = TEXT( "HealthBar" );
}

namespace NetGame
{
	// Session Setting에 설정되는 Host Player Name
	const static FName G_HostPlayerNameKey = TEXT( "NetHostPlayerName" );

	// Travel URL에 붙이는 Player Name parameter의 Key
	const static FString G_PlayerNameParamKey = TEXT( "NetPlayerName" );

	// 현재 참여중인 Player가 몇명인지에 대한 URL Option Param key
	const static FString G_PlayerNumsParamKey = TEXT( "NetPlayerNums" );
}