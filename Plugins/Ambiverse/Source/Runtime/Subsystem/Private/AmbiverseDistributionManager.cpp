// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseDistributionManager.h"
#include "AmbiverseDistributorAsset.h"
#include "AmbiverseElementInstance.h"
#include "AmbiverseElementAsset.h"
#include "AmbiverseSoundSourceManager.h"
#include "AmbiverseSubsystem.h"
#include "AmbiverseUtilities.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseDistributionManager, LogAmbiverseDistributionManager);

bool UAmbiverseDistributionManager::GetTransformForElement(FTransform& Transform, UAmbiverseElementInstance* ElementInstance)
{
	if (!ElementInstance || !Owner) { return false; }

	FTransform ListenerTransform;
	if (!AmbiverseUtilities::GetListenerTransform(ListenerTransform))
	{
		return false;
	}
	
	/** We first check if the element implements a custom distributor object. */
	if (const TSubclassOf<UAmbiverseDistributorAsset> DistributorClass {ElementInstance->RuntimeData.ElementAsset->GetDistributorClass()})
	{
		if (UAmbiverseDistributorAsset* Distributor {GetDistributorByClass(DistributorClass)})
		{
			if (Distributor->ExecuteDistribution(Transform, ListenerTransform.GetLocation(), ElementInstance->RuntimeData.ElementAsset))
			{
				return true;
			}
		}
	}
	/** If the element does not implement a custom distributor object, we perform the distribution here. */
	else
	{
		const FAmbiverseSoundDistributionData& SoundDistributionData {ElementInstance->RuntimeData.ElementAsset->GetDistributionData()};
		TArray<FVector> SoundSourcePlayLocations;

		switch (SoundDistributionData.DistributionMode)
		{
		case EDistributionMode::Random:
			return PerformRandomDistribution(Transform, ListenerTransform, ElementInstance);
		
		case EDistributionMode::Uniform:
			/** TODO: For now, we take all elements for our uniform distribution. Implement element-type specific uniform distribution. */
			if (const UAmbiverseSoundSourceManager* SoundSourceManager {Owner->GetSoundSourceManager()})
			{
				TArray<AAmbiverseSoundSource*> ActiveSoundSources {SoundSourceManager->GetActiveSoundSources()};
				for (const AAmbiverseSoundSource* ActiveSoundSource :ActiveSoundSources)
				{
					SoundSourcePlayLocations.Add(ActiveSoundSource->GetActorLocation());
				}
			}
			return PerformUniformDistribution(Transform, ListenerTransform, ElementInstance, SoundSourcePlayLocations, true);

		case EDistributionMode::Static:
			return PerformStaticDistribution(Transform, ListenerTransform, ElementInstance);
		}
	}
	return false;
}

UAmbiverseDistributorAsset* UAmbiverseDistributionManager::GetDistributorByClass(TSubclassOf<UAmbiverseDistributorAsset> Class)
{
	if (!Owner) { return nullptr ; }
	
	for (UAmbiverseDistributorAsset* Distributor : Distributors)
	{
		if (Distributor->StaticClass() == Class)
		{
			UE_LOG(LogAmbiverseDistributionManager, VeryVerbose, TEXT("GetDistributorByClass: Found existing distributor of class: %s"), *Class->GetName());
			return Distributor;
		}
	}

	/** If no instance of the specified distributor class was found, we instance a new one and return it. */
	UAmbiverseDistributorAsset* Distributor {NewObject<UAmbiverseDistributorAsset>(this, Class.Get())};
	
	UE_LOG(LogAmbiverseDistributionManager, Verbose, TEXT("GetDistributorByClass: Created new distributor of class: %s"), *Class->GetName());

	if (Distributor)
	{
		Distributor->Initialize(Owner);
	}
	
	return Distributor;
}

bool UAmbiverseDistributionManager::PerformRandomDistribution(FTransform& OutTransform,
	const FTransform& ListenerTransform, UAmbiverseElementInstance* ElementInstance)
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

bool UAmbiverseDistributionManager::PerformUniformDistribution(FTransform& OutTransform,
	const FTransform& ListenerTransform, UAmbiverseElementInstance* ElementInstance,
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

bool UAmbiverseDistributionManager::PerformStaticDistribution(FTransform& OutTransform,
	const FTransform& ListenerTransform, UAmbiverseElementInstance* ElementInstance)
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


