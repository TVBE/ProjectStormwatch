﻿// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "DistributionWorker.h"
#include "..\Public\AmbiverseDistributor.h"
#include "AmbiverseElementAsset.h"
#include "AudioDevice.h"

bool DistributionWorker::GetTransformForElement(FTransform& OutTransform, const FAmbiverseElement& Element,
	const UObject* WorldContextObject)
{
	if (!Element.IsValid()) { return false; }

	FTransform ListenerTransform;
	GetListenerTransform(ListenerTransform);

	const DistributionParamameters DistributionParams {
		DistributionParamameters(OutTransform, ListenerTransform, Element, WorldContextObject)};

	/** First we check if the element has a custom distributor class set.
	 *	If this is the case, we'll instantiate the distributor and let it handle the distribution.
	 *	If the distributor class is null, we'll use one of the base distribution formulas instead. */
	if (const TSubclassOf<UAmbiverseDistributor> DistributorClass {Element.Asset->GetDistributorClass()})
	{
		return PerformDistributorDistribution(DistributionParams, DistributorClass);
	}
	
	switch (Element.DistributionData.DistributionMode)
	{
	case EElementDistributionMode::Random:
		return PerformRandomDistribution(DistributionParams);

	case EElementDistributionMode::Uniform:
		return PerformUniformDistribution(DistributionParams);

	case EElementDistributionMode::Static:
		return PerformStaticDistribution(DistributionParams);
	}

	return false;
}

inline double GetRandomAngle()
{
	return FMath::RandRange(0.0, 2.0 * PI);
}

inline double GetRadiusInRange(const FElementDistributionData& DistributionData)
{
	return FMath::RandRange(DistributionData.HorizontalRange.X, DistributionData.HorizontalRange.Y);
}

inline FVector ComputeRandomXYZ(const FElementDistributionData& DistributionData, const double Angle, const double Radius)
{
	const double X = Radius * FMath::Cos(Angle);
	const double Y = Radius * FMath::Sin(Angle);
    
	double Z = FMath::RandRange(DistributionData.VerticalRange * -0.5, DistributionData.VerticalRange * 0.5);
	Z += DistributionData.VerticalOffset;

	return FVector(X, Y, Z);
}

bool DistributionWorker::PerformRandomDistribution(const DistributionParamameters& Params)
{
	const FElementDistributionData& DistributionData {Params.Element.DistributionData};

	const double Angle {GetRandomAngle()};
	const double Radius {GetRadiusInRange(DistributionData)};
	
	const FVector Location {ComputeRandomXYZ(DistributionData, Angle, Radius) + Params.ListenerTransform.GetLocation()};

	Params.OutTransform.SetLocation(Location);

	return true;
}

inline FVector ComputeDrift(const FElementDistributionData& DistributionData)
{
	auto ComputeAxisDriftValue = [&]()
	{
		return FMath::RandRange(-0.5f * DistributionData.Drift, 0.5f * DistributionData.Drift);
	};
	return FVector(ComputeAxisDriftValue(), ComputeAxisDriftValue(), ComputeAxisDriftValue());
}

bool DistributionWorker::PerformStaticDistribution(const DistributionParamameters& Params)
{
	const FElementDistributionData& DistributionData {Params.Element.DistributionData};
	
	if (!Params.Element.LastPlayTransform.IsSet())
	{
		return PerformRandomDistribution(Params);
	}
	
	if (const float Distance {static_cast<float>(FVector::Dist(Params.ListenerTransform.GetLocation(),
		Params.Element.LastPlayTransform.GetValue().GetLocation()))};
		Distance >= DistributionData.Threshold)
	{
		return PerformRandomDistribution(Params);
	}
	
	const FVector Drift {ComputeDrift(DistributionData)};
	Params.OutTransform.SetLocation(Params.Element.LastPlayTransform.GetValue().GetLocation() + Drift);

	return true;
}

bool DistributionWorker::PerformDistributorDistribution(const DistributionParamameters& Params,
                                                        const TSubclassOf<UAmbiverseDistributor> Distributor)
{
	if (!Distributor) { return false; }

	UWorld* World {Params.WorldContextObject->GetWorld()};
	if (!World) { return false; }
	
	UAmbiverseDistributor* DistributorInstance {NewObject<UAmbiverseDistributor>(World, Distributor)};
	if (!DistributorInstance) { return false; }

	DistributorInstance->Initialize(Params.WorldContextObject);
	
	const bool Success {DistributorInstance->ExecuteDistribution(
		Params.OutTransform, Params.ListenerTransform.GetLocation(), Params.Element.GetAsset())};
	
	return Success;
}

inline FVector ComputeXYDirection(const FVector& Vector, const FVector& ListenerLocation)
{
	FVector RelativeVector = Vector - ListenerLocation;
	RelativeVector.Z = 0;
	RelativeVector.Normalize();
	return RelativeVector;
}

bool DistributionWorker::PerformUniformDistribution(const DistributionParamameters& Params)
{
	return false; //TODO: Implement this.
	// const FElementDistributionData& DistributionData {Params.Element.DistributionData};
	//
	//
	//
	// TArray<float> AngleHistogram;
	//
	// /** Initialize histogram with zeros. */
	// AngleHistogram.Init(0, 360);  
	//
	// for (const FVector& Vector : Vectors)
	// {
	// 	FVector RelativeVector {ComputeXYDirection(Vector, Params.ListenerTransform.GetLocation())};
	//
	// 	float Angle = FMath::RadiansToDegrees(FMath::Atan2(RelativeVector.Y, RelativeVector.X));
	// 	if (Angle < 0) Angle += 360;
	//
	// 	/** Increment the angle bin. */
	// 	AngleHistogram[static_cast<int>(Angle)]++;  
	// }
	//
	// /** Find the least occupied angle. */
	// int LeastOccupiedAngle {0};
	// for (int i {0}; i < 360; i++)
	// {
	// 	if (AngleHistogram[i] < AngleHistogram[LeastOccupiedAngle])
	// 	{
	// 		LeastOccupiedAngle = i;
	// 	}
	// }
	//
	// const float Angle {FMath::DegreesToRadians(static_cast<float>(LeastOccupiedAngle))};  // Convert back to radians.
 //    
	// const float Radius {static_cast<float>(FMath::RandRange(DistributionData.HorizontalRange.X, DistributionData.HorizontalRange.Y))};
	//
	// const float X {Radius * FMath::Cos(Angle)};
	// const float Y {Radius * FMath::Sin(Angle)};
	//
	// float Z {static_cast<float>(FMath::RandRange(DistributionData.VerticalRange * -0.5, DistributionData.VerticalRange * 0.5))};
	// Z += DistributionData.VerticalOffset;
	//
	// FVector Location {FVector(X, Y, Z) + ListenerTransform.GetLocation()};
	//
	// const FVector Drift {
	// 	FMath::RandRange(-0.5f * DistributionData.Drift, 0.5f * DistributionData.Drift),
	// 	FMath::RandRange(-0.5f * DistributionData.Drift, 0.5f * DistributionData.Drift),
	// 	0.0f
	// };
	//
	// Location += Drift; // Add drift to the calculated location
	//
	// OutTransform.SetLocation(Location);
	//
	// return true;
}

bool DistributionWorker::GetListenerTransform(FTransform& Transform)
{
	if (const FAudioDevice* AudioDevice {GEngine->GetMainAudioDevice()}; AudioDevice->GetListeners().Num() > 0)
	{
		Transform = AudioDevice->GetListeners()[0].Transform;
		return true;
	}
	return false;
}

// void UAmbiverseDistributorAsset::SnapToFloor(FVector& Location, float Offset)
// {
// 	if (!World)
// 	{
// 		return;
// 	}
// 	
// 	const FVector StartLocation {Listener->GetActorLocation()};
// 	const FVector EndLocation {StartLocation + FVector{0.0f, 0.0f, -FLT_MAX}};
//
// 	FHitResult HitResult;
// 	FCollisionQueryParams TraceParams(FName(TEXT("SnapToFloorTrace")), true, Listener);
// 	TraceParams.bTraceComplex = false;
//
// 	const bool Hit {World->LineTraceSingleByChannel(
// 	HitResult,
// 	StartLocation,
// 	EndLocation,
// 	ECC_Visibility,
// 	TraceParams
// 	)};
//
// 	if (Hit)
// 	{
// 		Location = HitResult.Location + FVector(0, 0, Offset);
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("SnapToFloor: Failed to snap to floor."));
// 	}
// }

