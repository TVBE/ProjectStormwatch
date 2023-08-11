// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "DistributionWorker.h"
#include "AmbiverseDistributorAsset.h"
#include "AmbiverseElementAsset.h"
#include "AudioDevice.h"

bool DistributionWorker::GetTransformForElement(FTransform& OutTransform, const FAmbiverseElement& Element,
	const UObject* WorldContextObject)
{
	if (!Element.IsValid()) { return false; }

	FTransform ListenerTransform;
	GetListenerTransform(ListenerTransform);

	/** First we check if the element has a custom distributor class set.
	 *	If this is the case, we'll instantiate the distributor and let it handle the distribution.
	 *	If the distributor class is null, we'll use one of the base distribution formulas instead. */
	if (const TSubclassOf<UAmbiverseDistributorAsset> DistributorClass {Element.Asset->GetDistributorClass()})
	{
		return PerformDistributorDistribution(OutTransform, ListenerTransform, DistributorClass);
	}
	
	switch (Element.DistributionData.DistributionMode)
	{
	case EElementDistributionMode::Random:
		return PerformRandomDistribution(OutTransform, ListenerTransform, Element);

	case EElementDistributionMode::Uniform:
		return PerformUniformDistribution(OutTransform, ListenerTransform, Element, true, WorldContextObject);

	case EElementDistributionMode::Static:
		return PerformStaticDistribution(OutTransform, ListenerTransform, Element);
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

bool DistributionWorker::PerformRandomDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
	const FAmbiverseElement& Element)
{
	const FElementDistributionData& DistributionData {Element.DistributionData};

	const double Angle {GetRandomAngle()};
	const double Radius {GetRadiusInRange(DistributionData)};
	
	const FVector Location {ComputeRandomXYZ(DistributionData, Angle, Radius) + ListenerTransform.GetLocation()};

	OutTransform.SetLocation(Location);

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

bool DistributionWorker::PerformStaticDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
	const FAmbiverseElement& Element)
{
	const FElementDistributionData& DistributionData {Element.DistributionData};
	
	if (!Element.LastPlayTransform.IsSet())
	{
		return PerformRandomDistribution(OutTransform, ListenerTransform, Element);
	}
	
	if (const float Distance {static_cast<float>(FVector::Dist(ListenerTransform.GetLocation(),
		Element.LastPlayTransform.GetValue().GetLocation()))};
		Distance >= DistributionData.Threshold)
	{
		return PerformRandomDistribution(OutTransform, ListenerTransform, Element);
	}
	
	const FVector Drift {ComputeDrift(DistributionData)};
	OutTransform.SetLocation(Element.LastPlayTransform.GetValue().GetLocation() + Drift);

	return true;
}

bool DistributionWorker::PerformDistributorDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
                                                      TSubclassOf<UAmbiverseDistributorAsset> Distributor)
{
	return false;
}

// bool UAmbiverseDistributorPool::PerformUniformDistribution(FTransform& OutTransform,
// 	const FTransform& ListenerTransform, UAmbiverseElement* ElementInstance,
// 	const TArray<FVector>& Vectors, const bool IgnoreZ = true)
// {
// 	return;
//
// 	const FElementDistributionData& DistributionData {ElementInstance->RuntimeData.ElementAsset->GetDistributionData()};
// 	
// 	TArray<float> AngleHistogram;
// 	
// 	/** Initialize histogram with zeros. */
// 	AngleHistogram.Init(0, 360);  
//
// 	for (const FVector& Vector : Vectors)
// 	{
// 		FVector RelativeVector = Vector - ListenerTransform.GetLocation();
// 		if (IgnoreZ) RelativeVector.Z = 0;
// 		RelativeVector.Normalize();
//
// 		float Angle = FMath::RadiansToDegrees(FMath::Atan2(RelativeVector.Y, RelativeVector.X));
// 		if (Angle < 0) Angle += 360;
//
// 		/** Increment the angle bin. */
// 		AngleHistogram[static_cast<int>(Angle)]++;  
// 	}
//
// 	/** Find the least occupied angle. */
// 	int LeastOccupiedAngle {0};
// 	for (int i {0}; i < 360; i++)
// 	{
// 		if (AngleHistogram[i] < AngleHistogram[LeastOccupiedAngle])
// 		{
// 			LeastOccupiedAngle = i;
// 		}
// 	}
//
// 	const float Angle {FMath::DegreesToRadians(static_cast<float>(LeastOccupiedAngle))};  // Convert back to radians.
//     
// 	const float Radius {static_cast<float>(FMath::RandRange(DistributionData.HorizontalRange.X, DistributionData.HorizontalRange.Y))};
//
// 	const float X {Radius * FMath::Cos(Angle)};
// 	const float Y {Radius * FMath::Sin(Angle)};
//
// 	float Z {static_cast<float>(FMath::RandRange(DistributionData.VerticalRange * -0.5, DistributionData.VerticalRange * 0.5))};
// 	Z += DistributionData.VerticalOffset;
//
// 	FVector Location {FVector(X, Y, Z) + ListenerTransform.GetLocation()};
// 	
// 	const FVector Drift {
// 		FMath::RandRange(-0.5f * DistributionData.Drift, 0.5f * DistributionData.Drift),
// 		FMath::RandRange(-0.5f * DistributionData.Drift, 0.5f * DistributionData.Drift),
// 		0.0f
// 	};
//
// 	Location += Drift; // Add drift to the calculated location
//
// 	OutTransform.SetLocation(Location);
//
// 	return true;
// }



bool DistributionWorker::GetListenerTransform(FTransform& Transform)
{
	if (const FAudioDevice* AudioDevice {GEngine->GetMainAudioDevice()}; AudioDevice->GetListeners().Num() > 0)
	{
		Transform = AudioDevice->GetListeners()[0].Transform;
		return true;
	}
	return false;
}

UAmbiverseSubsystem* DistributionWorker::GetSubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject || !WorldContextObject->ImplementsGetWorld()) { return nullptr; }
	return GetSubsystem(WorldContextObject->GetWorld());
}

UAmbiverseSubsystem* DistributionWorker::GetSubsystem(const UWorld* World)
{
	if (!World) { return nullptr; }
	return World->GetSubsystem<UAmbiverseSubsystem>();
}
