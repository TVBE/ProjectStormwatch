// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "SensoryEventManager.h"
#include "HeatPoint.h"
#include "Nightstalker.h"
#include "NightstalkerDirector.h"

DEFINE_LOG_CATEGORY_CLASS(USensoryEventManager, LogSensoryEventManager);

void USensoryEventManager::Initialize(UNightstalkerDirector* Subsystem)
{
	if (!Subsystem) { return; }

	Director = Subsystem;
	
	if (const UWorld* World {GetWorld()})
	{
		World->GetTimerManager().SetTimer(AuditoryEventProcessorTimerHandle, this, &USensoryEventManager::ProcessAuditoryEvents, 5.0f, true);
		UE_LOG(LogSensoryEventManager, Log, TEXT("Initialized sensory event manager."))
	}
}

void USensoryEventManager::Deinitialize()
{
	if (const UWorld* World {GetWorld()})
	{
		if (World->GetTimerManager().IsTimerActive(AuditoryEventProcessorTimerHandle))
		{
			World->GetTimerManager().ClearTimer(AuditoryEventProcessorTimerHandle);
		}
	}
}

void USensoryEventManager::AddAuditoryEventAtLocation(FAuditoryEvent Event, const FVector& Location)
{
	Event.Location = Location;
	AuditoryEventQueue.Add(Event);
}

inline FAuditoryEvent CombineAuditoryEvents(const FAuditoryEvent& EventA, const FAuditoryEvent& EventB)
{
	FAuditoryEvent CombinedEvent;
	CombinedEvent.Location = (EventA.Location + EventB.Location) / 2.0f;
	CombinedEvent.Loudness = FMath::Loge(FMath::Exp(EventA.Loudness) + FMath::Exp(EventB.Loudness));
	return CombinedEvent;
}

inline void AttenuateLoudness(float& Loudness, const FVector& Origin, const FVector& Target)
{
	const float Distance {static_cast<float>(FVector::Dist(Origin, Target))};

	const float AttenuationFactor {20.0f * FMath::LogX(10.0f, 1.0f + Distance / 300.0f)};

	Loudness -= AttenuationFactor;
}

inline float LoudnessToHeat(float Loudness)
{
	const float Interest = FMath::GetMappedRangeValueClamped(FVector2D(30, 100), FVector2D(10, 100), Loudness);

	return Interest;
}

inline float GetHeatPointRadius(const FVector& VectorA, const FVector& VectorB)
{
	constexpr float MinDistance {1000.0f};
	constexpr float MaxDistance {4000.0f};
	constexpr float MinRadius {300.0f};
	constexpr float MaxRadius {1000.0f};
	
	const float Distance {static_cast<float>(FVector::Dist(VectorA, VectorB))};
	const float Radius {static_cast<float>(FMath::GetMappedRangeValueClamped(FVector2D(MinDistance, MaxDistance), FVector2D(MinRadius, MaxRadius), Distance))};
	return Radius;
}

void USensoryEventManager::ProcessAuditoryEvents()
{
	UE_LOG(LogSensoryEventManager, Verbose, TEXT("Processor Update:"))
	
	if (!Nightstalker)
	{
		UE_LOG(LogSensoryEventManager, Verbose, TEXT("Skipped processing auditory events as no Nightstalker instance exists."))
		return;
	}
	if (AuditoryEventQueue.Num() == 0)
	{
		UE_LOG(LogSensoryEventManager, Verbose, TEXT("Skipped processing auditory events as queue is empty."))
		return;
	}

	UE_LOG(LogSensoryEventManager, Verbose, TEXT("Processing auditory events."))

	TArray<FAuditoryEvent> CombinedEvents;
	constexpr float CombineRadiusSquared {FMath::Square(300.0f)};

	for (const FAuditoryEvent& CurrentEvent : AuditoryEventQueue)
	{
		bool IsCombined {false};
		for (FAuditoryEvent& ExistingEvent : CombinedEvents)
		{
			if (const float DistanceSquared {static_cast<float>(FVector::DistSquared(CurrentEvent.Location, ExistingEvent.Location))};
				DistanceSquared <= CombineRadiusSquared)
			{
				ExistingEvent = CombineAuditoryEvents(ExistingEvent, CurrentEvent);
				IsCombined = true;
				break;
			}
		}

		if (!IsCombined)
		{
			CombinedEvents.Add(CurrentEvent);
		}
	}

	UE_LOG(LogSensoryEventManager, Verbose, TEXT("Combined '%d' auditory events to '%d' events."), AuditoryEventQueue.Num(), CombinedEvents.Num())

	AuditoryEventQueue.Empty();

	TArray<FHeatAtLocation> HeatAtLocations;
	
	for (FAuditoryEvent& AuditoryEvent : CombinedEvents)
	{
		AttenuateLoudness(AuditoryEvent.Loudness, AuditoryEvent.Location, Nightstalker->GetActorLocation());

		HeatAtLocations.Add(FHeatAtLocation(LoudnessToHeat(AuditoryEvent.Loudness), AuditoryEvent.Location));
	}

	UE_LOG(LogSensoryEventManager, Verbose, TEXT("Attenuated auditory events to heat points: '%d'"), CombinedEvents.Num())

	TArray<FHeatAtLocation> IsolatedHeatAtLocations;

	for (const FHeatAtLocation& HeatLocation : HeatAtLocations)
	{
		FCollisionQueryParams Params;
		Params.bTraceComplex = false;
		Params.TraceTag = "HeatOverlapCheck";
	
		TArray<FOverlapResult> Overlaps;
		const bool IsOverlapping = GetWorld()->OverlapMultiByChannel(Overlaps, HeatLocation.Location, FQuat::Identity, ECC_GameTraceChannel3, FCollisionShape::MakeSphere(0.1f), Params);
	
		AHeatPoint* OverlappingHeatPoint {nullptr};
		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (AHeatPoint* HeatPoint {Cast<AHeatPoint>(Overlap.GetActor())})
			{
				OverlappingHeatPoint = HeatPoint;
				break;
			}
		}

		if (OverlappingHeatPoint)
		{
			OverlappingHeatPoint->AddHeat(HeatLocation.Heat);
		}
		else
		{
			IsolatedHeatAtLocations.Add(HeatLocation);
		}
	}

	HeatAtLocations = MoveTemp(IsolatedHeatAtLocations);

	if (!HeatAtLocations.Num() == 0)
	{
		for (const FHeatAtLocation& HeatLocation : HeatAtLocations)
		{
			if (AHeatPoint* NewHeatPoint {GetWorld()->SpawnActor<AHeatPoint>(AHeatPoint::StaticClass(), HeatLocation.Location, FRotator::ZeroRotator)})
			{
				NewHeatPoint->InitializeHeatPoint(GetHeatPointRadius(HeatLocation.Location, Nightstalker->GetActorLocation()), 60, HeatLocation.Heat);
				
				if (Director)
				{
					Director->RegisterHeatPoint(NewHeatPoint);
				}
			}
		}
	}
}
