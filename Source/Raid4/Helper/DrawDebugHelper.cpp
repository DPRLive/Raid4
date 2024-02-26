#include "DrawDebugHelper.h"

#include <Components/LineBatchComponent.h>
#include <Engine/GameViewportClient.h>
#include <DrawDebugHelpers.h>

#if UE_ENABLE_DEBUG_DRAWING

float GServerDrawDebugColorTintStrength = 0.75f;
FLinearColor GServerDrawDebugColorTint(0.0f, 0.0f, 0.0f, 1.0f);

#if WITH_EDITOR

FColor AdjustColorForServer(const FColor InColor)
{
	if (GServerDrawDebugColorTintStrength > 0.0f)
	{
		return FMath::Lerp(FLinearColor::FromSRGBColor(InColor), GServerDrawDebugColorTint, GServerDrawDebugColorTintStrength).ToFColor(/*bSRGB=*/ true);
	}
	else
	{
		return InColor;
	}
}

bool CanDrawServerDebugInContext(const FWorldContext& WorldContext)
{
	return
		(WorldContext.WorldType == EWorldType::PIE) &&
		(WorldContext.World() != nullptr) &&
		(WorldContext.World()->GetNetMode() == NM_Client) &&
		(WorldContext.GameViewport != nullptr) &&
		(WorldContext.GameViewport->EngineShowFlags.ServerDrawDebug);
}

#define UE_DRAW_SERVER_DEBUG_ON_EACH_CLIENT(FunctionName, ...) \
	if (GIsEditor) \
	{ \
		for (const FWorldContext& WorldContext : GEngine->GetWorldContexts()) \
		{ \
			if (CanDrawServerDebugInContext(WorldContext)) \
			{ \
				FunctionName(WorldContext.World(), __VA_ARGS__); \
			} \
		} \
	}

#else

#define UE_DRAW_SERVER_DEBUG_ON_EACH_CLIENT(FunctionName, ...)

#endif

ULineBatchComponent* GetDebugLineBatcher( const UWorld* InWorld, bool bPersistentLines, float LifeTime, bool bDepthIsForeground )
{
	return (InWorld ? (bDepthIsForeground ? InWorld->ForegroundLineBatcher : (( bPersistentLines || (LifeTime > 0.f) ) ? InWorld->PersistentLineBatcher : InWorld->LineBatcher)) : nullptr);
}

static float GetDebugLineLifeTime(ULineBatchComponent* LineBatcher, float LifeTime, bool bPersistent)
{
	return bPersistent ? -1.0f : ((LifeTime > 0.f) ? LifeTime : LineBatcher->DefaultLifeTime);
}

/**
* 기존 DrawDebugCircleArc에서 Y축을 추가한 함수 
*/
void DrawDebugHelper::DrawDebugCircleArcXY(const UWorld* InWorld, const FVector& InCenter, float InRadius, const FVector& InAxisX, const FVector& InAxisY, float InAngleWidth, int32 InSegments, const FColor& InColor, bool InPersistentLines, float InLifeTime, uint8 InDepthPriority, float InThickness)
{
	if (GEngine->GetNetMode(InWorld) != NM_DedicatedServer)
	{
		if (ULineBatchComponent* const LineBatcher = GetDebugLineBatcher(InWorld, InPersistentLines, InLifeTime, (InDepthPriority == SDPG_Foreground)))
		{
			const float LineLifeTime = GetDebugLineLifeTime(LineBatcher, InLifeTime, InPersistentLines);

			// Need at least 4 segments
			InSegments = FMath::Max(InSegments, 4);
			const float AngleStep = InAngleWidth / float(InSegments) * 2.f;

			FVector AxisX = InAxisX.GetSafeNormal();
			FVector AxisY = InAxisY.GetSafeNormal();
			
			TArray<FBatchedLine> Lines;
			Lines.Empty(InSegments);
			float Angle = -InAngleWidth;
			FVector PrevVertex = InCenter + InRadius * (AxisY * -FMath::Sin(Angle) + AxisX * FMath::Cos(Angle));
			while (InSegments--)
			{
				Angle += AngleStep;
				FVector NextVertex = InCenter + InRadius * (AxisY * -FMath::Sin(Angle) + AxisX * FMath::Cos(Angle));
				Lines.Emplace(FBatchedLine(PrevVertex, NextVertex, InColor, LineLifeTime, InThickness, InDepthPriority));
				PrevVertex = NextVertex;
			}

			LineBatcher->DrawLines(Lines);
		}
	}
	else
	{
		UE_DRAW_SERVER_DEBUG_ON_EACH_CLIENT(DrawDebugCircleArcXY, InCenter, InRadius, InAxisX, InAxisY, InAngleWidth, InSegments, AdjustColorForServer(InColor), InPersistentLines, InLifeTime, InDepthPriority, InThickness);
	}
}

/**
* Sector (부채꼴) 형태를 draw 한다.
* @param InTrans : Sector의 Transform
* @param InRadius : Sector의 반지름
* @param InHalfHeight : Sector의 반 높이
* @param InRadian : Sector의 내부 각도 (Radian)
*/
void DrawDebugHelper::DrawDebugSector(const UWorld* InWorld, const FTransform& InTrans, float InRadius, float InHalfHeight, float InRadian, int32 InSegments, const FColor& InColor, bool InPersistentLines, float InLifeTime, uint8 InDepthPriority, float InThickness)
{
	const FVector centerUp = InTrans.TransformPosition(FVector::UpVector * InHalfHeight); // 중심 위
	const FVector centerDown = InTrans.TransformPosition(FVector::DownVector * InHalfHeight); // 중심 아래
	const FVector arcDir1 = InTrans.GetUnitAxis(EAxis::X).RotateAngleAxisRad(InRadian / 2, InTrans.GetUnitAxis(EAxis::Z)); // 호의 시계 방향 쪽 끝을 향하는 방향 벡터
	const FVector arcDir2 = InTrans.GetUnitAxis(EAxis::X).RotateAngleAxisRad(-InRadian / 2, InTrans.GetUnitAxis(EAxis::Z)); // 호의 반시계 방향 쪽 끝을 향하는 방향 벡터
	const FVector upPoint1 = centerUp + arcDir1 * InRadius; // 위 호의 point 1
	const FVector upPoint2 = centerUp + arcDir2 * InRadius; // 위 호의 point 2
	const FVector dwnPoint1 = centerDown + arcDir1 * InRadius; // 아래 호의 point 1
	const FVector dwnPoint2 = centerDown + arcDir2 * InRadius; // 아래 호의 point 2
	
	// 위 아래 호 두개 그리기
	DrawDebugCircleArcXY(InWorld, centerUp, InRadius, InTrans.GetUnitAxis(EAxis::X), InTrans.GetUnitAxis(EAxis::Y), InRadian / 2, InSegments, InColor, InPersistentLines, InLifeTime);
	DrawDebugCircleArcXY(InWorld, centerDown, InRadius, InTrans.GetUnitAxis(EAxis::X), InTrans.GetUnitAxis(EAxis::Y), InRadian / 2, InSegments, InColor, InPersistentLines, InLifeTime);
	// 중심 위 <-> 아래 잇는 선분
	DrawDebugLine(InWorld, centerUp, centerDown, InColor, InPersistentLines, InLifeTime);
	// 중심 위 <-> 위 호의 양 끝을 잇는 선분
	DrawDebugLine(InWorld, centerUp, upPoint1, InColor, InPersistentLines, InLifeTime);
	DrawDebugLine(InWorld, centerUp, upPoint2, InColor, InPersistentLines, InLifeTime);
	// 중심 아래 <-> 아래 호의 양 끝을 잇는 선분
	DrawDebugLine(InWorld, centerDown, dwnPoint1, InColor, InPersistentLines, InLifeTime);
	DrawDebugLine(InWorld, centerDown, dwnPoint2, InColor, InPersistentLines, InLifeTime);
	// 위 아래 호 양끝을 잇는 선분
	DrawDebugLine(InWorld, dwnPoint1, upPoint1, InColor, InPersistentLines, InLifeTime);
	DrawDebugLine(InWorld, dwnPoint2, upPoint2, InColor, InPersistentLines, InLifeTime);
}

#endif
