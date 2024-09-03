#include "UtilAnimation.h"

#include <Animation/AnimMontage.h>

/**
 *  AnimMontage의 링크를 포함한 특정 Section에 대한 총 시간을 측정
 *  @param InAnimMontage : 시간을 측정할 대상 Anim Montage
 *  @param InSectionIndex : 시간을 측정할 Section의 시작 index
 *  @return : 총 시간, 루프시 -1 반환
 */
float UtilAnimation::GetCompositeAnimLength(UAnimMontage* InAnimMontage, int32 InSectionIndex)
{
	if(!IsValid(InAnimMontage) || InSectionIndex == INDEX_NONE)
		return 0.f;

	float totalLength = 0.f;
	
	TArray<bool> Chk;
	Chk.SetNum(InAnimMontage->GetNumSections());

	int32 nextIndex = InSectionIndex;
	do
	{
		Chk[nextIndex] = true;

		// 길이 갱신
		totalLength += InAnimMontage->GetSectionLength(nextIndex);
		
		// Next Section 검색
		const FName& nextSection = InAnimMontage->GetAnimCompositeSection(nextIndex).NextSectionName;
		nextIndex = InAnimMontage->GetSectionIndex(nextSection);
		
		if(Chk.IsValidIndex(nextIndex) && Chk[nextIndex]) // 사이클 탐지시 -1.f 반환
			return -1.f;
	}
	while(nextIndex != INDEX_NONE); // 다음 링크가 없을때 까지.

	return totalLength;
}

/**
 *  링크를 포함한 anim montage section의 delay된 시작 Pos 반환
 *  @param InAnimMontage : 대상 Anim Montage
 *  @param InSectionIndex : Section의 시작 index
 *  @param InDelay : 적용할 delay
 *  @return : delay가 적용된 시작 Pos, (InAnimMontage 가 null이거나, InSectionIndex가 INDEX_NONE이거나, delay가 총 길이보다 길면 -1 반환)
 */
float UtilAnimation::GetDelayedStartAnimPos(UAnimMontage* InAnimMontage, int32 InSectionIndex, float InDelay)
{
	if(!IsValid(InAnimMontage) || InSectionIndex == INDEX_NONE)
		return -1.f;

	float startPos = InAnimMontage->GetAnimCompositeSection(InSectionIndex).GetTime();
	float nowLength = InAnimMontage->GetSectionLength(InSectionIndex);
	int32 nowIndex = InSectionIndex;
	
	while(InDelay > nowLength)
	{
		InDelay -= nowLength;
		
		// Next Section 검색
		const FName& nextSection = InAnimMontage->GetAnimCompositeSection(nowIndex).NextSectionName;

		// delay가 아직 남았는데 section이 끝나버리면 -1 반환
		if(nextSection == NAME_None)
			return -1.f;

		nowIndex = InAnimMontage->GetSectionIndex(nextSection);
		startPos = InAnimMontage->GetAnimCompositeSection(nowIndex).GetTime();
		nowLength = InAnimMontage->GetSectionLength(nowIndex);
	}

	return startPos + InDelay;
}