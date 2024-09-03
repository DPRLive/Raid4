#pragma once

class UAnimMontage;

namespace UtilAnimation
{
	// AnimMontage의 링크를 포함한 특정 Section에 대한 총 시간을 측정
	float GetCompositeAnimLength(UAnimMontage* InAnimMontage, int32 InSectionIndex);

	// 링크를 포함한 anim montage section의 delay된 시작 Pos 반환
	float GetDelayedStartAnimPos(UAnimMontage* InAnimMontage, int32 InSectionIndex, float InDelay);
};
