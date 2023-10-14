// Copyright Notice

#include "NightstalkerAIFunctionLibrary.h"

inline static const TArray<FVector2D> OcclusionTraceVectors = {
	FVector2D(-50, 50),
	FVector2D(50, 50),
	FVector2D(-50, -50),
	FVector2D(50, -50)
};

bool UNightstalkerAIFunctionLibrary::IsOccluded(UObject* WorldContextObject, const FVector& Origin,
	const FVector& PointB, const bool DrawDebugLines, const float DebugLineDuration)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return false;
	}

	FVector Direction =  (PointB - Origin).GetSafeNormal() ;
	FRotator Rotation =  Direction.Rotation() ;

	FCollisionQueryParams TraceParams;

	FHitResult HitResult;
	for (const FVector2D& TraceVector : OcclusionTraceVectors)
	{
		FVector Start { Rotation.RotateVector(FVector{0, TraceVector.X * 0.25, TraceVector.Y * 0.25}) + Origin };
		FVector End { Rotation.RotateVector(FVector{0, TraceVector.X, TraceVector.Y}) + PointB };
		
		bool IsHit =  World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams) ;
		if (!IsHit)
		{
			if (DrawDebugLines)
			{
				DrawDebugLine(World, Start, End, FColor::Red, false, DebugLineDuration);
			}
			return false;
		}
		else if (DrawDebugLines)
		{
			DrawDebugLine(World, Start, End, FColor::Green, false, DebugLineDuration);
		}
	}

	return true;
}

bool UNightstalkerAIFunctionLibrary::IsOccludedFast(UObject* WorldContextObject, const FVector& LocationA,
	const FVector& LocationB, const bool DrawDebugLines, const float DebugLineDuration)
{
	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return false;
	}

	FVector SourceTracePoints[2] {LocationA + FVector(0, 0, 50), LocationA - FVector(0, 0, 30)};
	FVector TargetTracePoints[2] {LocationB + FVector(0, 0, 50), LocationB - FVector(0, 0, 30)};

	FCollisionQueryParams TraceParams {};

	FHitResult HitResult;
	for (int i = 0; i < 2; ++i)
	{
		bool IsHit = World->LineTraceSingleByChannel(HitResult, SourceTracePoints[i], TargetTracePoints[i], ECC_Visibility, TraceParams);
		if (DrawDebugLines)
		{
			DrawDebugLine(World, SourceTracePoints[i], TargetTracePoints[i], IsHit ? FColor::Red : FColor::Green, false, DebugLineDuration);
		}
		if (IsHit)
		{
			return false;
		}
	}
	return true;
}

float UNightstalkerAIFunctionLibrary::GetViewAngle(const FRotator& Rotation, const FVector& Origin, const FVector& Target, bool IgnorePitch)
{
	FVector DirectionToTarget = (Target - Origin).GetSafeNormal();

	FVector ForwardVector = Rotation.Vector();

	if (IgnorePitch)
	{
		DirectionToTarget.Z = 0.0f;
		ForwardVector.Z = 0.0f;
		DirectionToTarget = DirectionToTarget.GetSafeNormal();
		ForwardVector = ForwardVector.GetSafeNormal();
	}
	
	float Angle = static_cast<float>(FMath::Acos(FVector::DotProduct(DirectionToTarget, ForwardVector)) * (180.0f / PI));

	return Angle;
}
