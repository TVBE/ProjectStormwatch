// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseStatics.h"
#include "AmbiverseDistributorAsset.h"
#include "AmbiverseElementAsset.h"
#include "AudioDevice.h"

bool AmbiverseStatics::GetTransformForElement(FTransform& OutTransform, const FAmbiverseElement& Element,
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

	const FAmbiverseSoundDistributionData& DistributionData {Element.Asset->GetDistributionData().DistributionMode};
	
	switch (DistributionData.DistributionMode)
	{
	case EDistributionMode::Random:
		return PerformRandomDistribution(OutTransform, ListenerTransform, Element);

	case EDistributionMode::Uniform:
		return PerformUniformDistribution(OutTransform, ListenerTransform, Element, true, WorldContextObject);

	case EDistributionMode::Static:
		return PerformStaticDistribution(OutTransform, ListenerTransform, Element);
	}
}

bool AmbiverseStatics::PerformRandomDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
	const FAmbiverseElement& Element)
{
}

bool AmbiverseStatics::PerformUniformDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
	const FAmbiverseElement& Element, const bool IgnoreZ, const UObject* WorldContextObject)
{
}

bool AmbiverseStatics::PerformStaticDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
	const FAmbiverseElement& Element)
{
}

bool AmbiverseStatics::PerformDistributorDistribution(FTransform& OutTransform, const FTransform& ListenerTransform,
                                                      TSubclassOf<UAmbiverseDistributorAsset> Distributor)
{
}  

bool UAmbiverseDistributorPool::PerformRandomDistribution(FTransform& OutTransform,
                                                          const FTransform& ListenerTransform, UAmbiverseElement* ElementInstance)
{
	if (!ElementInstance || !ElementInstance->RuntimeData.ElementAsset)
	{
		return false;
	}
	
	const FAmbiverseSoundDistributionData& DistributionData {ElementInstance->RuntimeData.ElementAsset->GetDistributionData()};

	const double Angle {FMath::RandRange(0.0, 2.0 * PI)};

	const double Radius {FMath::RandRange(DistributionData.HorizontalRange.X, DistributionData.HorizontalRange.Y)};

	const double X {Radius * FMath::Cos(Angle)};
	const double Y {Radius * FMath::Sin(Angle)};

	double Z {FMath::RandRange(DistributionData.VerticalRange * -0.5, DistributionData.VerticalRange * 0.5)};
	Z += DistributionData.VerticalOffset;

	const FVector Location {FVector(X, Y, Z) + ListenerTransform.GetLocation()};

	OutTransform.SetLocation(Location);

	return true;
}

bool UAmbiverseDistributorPool::PerformUniformDistribution(FTransform& OutTransform,
	const FTransform& ListenerTransform, UAmbiverseElement* ElementInstance,
	const TArray<FVector>& Vectors, const bool IgnoreZ = true)
{
	if (!ElementInstance || !ElementInstance->RuntimeData.ElementAsset)
	{
		return false;
	}
	
	const FAmbiverseSoundDistributionData& DistributionData {ElementInstance->RuntimeData.ElementAsset->GetDistributionData()};
	
	TArray<float> AngleHistogram;
	
	/** Initialize histogram with zeros. */
	AngleHistogram.Init(0, 360);  

	for (const FVector& Vector : Vectors)
	{
		FVector RelativeVector = Vector - ListenerTransform.GetLocation();
		if (IgnoreZ) RelativeVector.Z = 0;
		RelativeVector.Normalize();

		float Angle = FMath::RadiansToDegrees(FMath::Atan2(RelativeVector.Y, RelativeVector.X));
		if (Angle < 0) Angle += 360;

		/** Increment the angle bin. */
		AngleHistogram[static_cast<int>(Angle)]++;  
	}

	/** Find the least occupied angle. */
	int LeastOccupiedAngle {0};
	for (int i {0}; i < 360; i++)
	{
		if (AngleHistogram[i] < AngleHistogram[LeastOccupiedAngle])
		{
			LeastOccupiedAngle = i;
		}
	}

	const float Angle {FMath::DegreesToRadians(static_cast<float>(LeastOccupiedAngle))};  // Convert back to radians.
    
	const float Radius {static_cast<float>(FMath::RandRange(DistributionData.HorizontalRange.X, DistributionData.HorizontalRange.Y))};

	const float X {Radius * FMath::Cos(Angle)};
	const float Y {Radius * FMath::Sin(Angle)};

	float Z {static_cast<float>(FMath::RandRange(DistributionData.VerticalRange * -0.5, DistributionData.VerticalRange * 0.5))};
	Z += DistributionData.VerticalOffset;

	FVector Location {FVector(X, Y, Z) + ListenerTransform.GetLocation()};
	
	const FVector Drift {
		FMath::RandRange(-0.5f * DistributionData.Drift, 0.5f * DistributionData.Drift),
		FMath::RandRange(-0.5f * DistributionData.Drift, 0.5f * DistributionData.Drift),
		0.0f
	};

	Location += Drift; // Add drift to the calculated location

	OutTransform.SetLocation(Location);

	return true;
}

bool UAmbiverseDistributorPool::PerformStaticDistribution(FTransform& OutTransform,
	const FTransform& ListenerTransform, UAmbiverseElement* ElementInstance)
{
	const FAmbiverseSoundDistributionData& SoundDistributionData {ElementInstance->RuntimeData.ElementAsset->GetDistributionData()};
	if (!ElementInstance->LastPlaylocation.IsZero())
	{
		if (const float Distance {static_cast<float>(FVector::Dist(ListenerTransform.GetLocation(), ElementInstance->LastPlaylocation))};
			Distance >= SoundDistributionData.Threshold)
		{
			return PerformRandomDistribution(OutTransform, ListenerTransform, ElementInstance);
		}
		else
		{
			const FVector Drift {
				FMath::RandRange(-0.5f * SoundDistributionData.Drift, 0.5f * SoundDistributionData.Drift), 
				FMath::RandRange(-0.5f * SoundDistributionData.Drift, 0.5f * SoundDistributionData.Drift), 
				0.0f
			};
			OutTransform.SetLocation(ElementInstance->LastPlaylocation + Drift);
		}
	}
	else
	{
		return PerformRandomDistribution(OutTransform, ListenerTransform, ElementInstance);
	}
	return true;
}

bool AmbiverseStatics::GetListenerTransform(FTransform& Transform)
{
	if (const FAudioDevice* AudioDevice {GEngine->GetMainAudioDevice()}; AudioDevice->GetListeners().Num() > 0)
	{
		Transform = AudioDevice->GetListeners()[0].Transform;
		return true;
	}
	return false;
}

UAmbiverseSubsystem* AmbiverseStatics::GetSubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject || !WorldContextObject->ImplementsGetWorld()) { return nullptr; }
	return GetSubsystem(WorldContextObject->GetWorld());
}

UAmbiverseSubsystem* AmbiverseStatics::GetSubsystem(const UWorld* World)
{
	if (!World) { return nullptr; }
	return World->GetSubsystem<UAmbiverseSubsystem>();
}
