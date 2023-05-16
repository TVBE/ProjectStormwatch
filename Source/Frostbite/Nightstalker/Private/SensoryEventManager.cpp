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

	Loudness = FMath::Max(0.0f, Loudness - AttenuationFactor);
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

inline AHeatPoint* CheckForOverlaps(const FHeatEvent& HeatAtLocation, const FVector& ListenerLocation, UWorld* World)
{
	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	Params.TraceTag = "HeatOverlapCheck";

	const float TraceRadius {GetHeatPointRadius(HeatAtLocation.Location, ListenerLocation)};

	TArray<FOverlapResult> Overlaps;
	const bool IsOverlapping = World->OverlapMultiByChannel(Overlaps, HeatAtLocation.Location, FQuat::Identity, ECC_GameTraceChannel3, FCollisionShape::MakeSphere(0.1f), Params);

	AHeatPoint* OverlappingHeatPoint {nullptr};
	for (const FOverlapResult& Overlap : Overlaps)
	{
		if (AHeatPoint* HeatPoint {Cast<AHeatPoint>(Overlap.GetActor())})
		{
			OverlappingHeatPoint = HeatPoint;
			break;
		}
	}

	return OverlappingHeatPoint;
}

inline FHeatEvent CombineHeatEvents(const FHeatEvent& HeatEventA, const FHeatEvent& HeatEventB)
{
	FHeatEvent CombinedHeatEvent;
	CombinedHeatEvent.Location = (HeatEventA.Location + HeatEventB.Location) / 2.0f;
	CombinedHeatEvent.Heat = FMath::Max(HeatEventA.Heat, HeatEventB.Heat);

	const float Distance = FVector::Dist(HeatEventA.Location, HeatEventB.Location);
	float CombinedRadius = FMath::Max(Distance + HeatEventA.Radius, HeatEventB.Radius);
	CombinedRadius = FMath::Clamp(CombinedRadius, 0.0f, 1500.0f);
	CombinedHeatEvent.Radius = CombinedRadius;

	return CombinedHeatEvent;
}

inline TArray<FHeatEvent> ConsolidateHeatEvents(const TArray<FHeatEvent>& HeatEvents, const FVector& ListenerLocation)
{
	TArray<FHeatEvent> NonOverlappingHeatEvents;
	
	for (const FHeatEvent& HeatEvent : HeatEvents)
	{
		const float HeatPointRadiusSquared = FMath::Square(GetHeatPointRadius(HeatEvent.Location, ListenerLocation));
		bool IsOverlapping {false};

		for (FHeatEvent& ExistingHeatEvent : NonOverlappingHeatEvents)
		{
			if (FVector::DistSquared(HeatEvent.Location, ExistingHeatEvent.Location) <= HeatPointRadiusSquared)
			{
				ExistingHeatEvent = CombineHeatEvents(ExistingHeatEvent, HeatEvent);
				IsOverlapping = true;
				break;
			}
		}

		if (!IsOverlapping)
		{
			NonOverlappingHeatEvents.Add(HeatEvent);
		}
	}

	return NonOverlappingHeatEvents;
}

void USensoryEventManager::ProcessAuditoryEvents()
{
	if (!Director || !Director->GetHeatPointManager()) { return; }
	
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

	
	/* For each event, we attempt to combine it with an existing event in the `ProcessedEvents` array.
	 * This combination is performed if the current event is within a certain distance from an existing event.
	 * If the current event cannot be combined with any existing event, it's added as a new event in `ProcessedEvents`.
	 * This process ensures that auditory events occurring close together are handled as a single combined event for the rest of the process. */
	TArray<FAuditoryEvent> ProcessedEvents;
	constexpr float CombineRadiusSquared {FMath::Square(400.0f)};

	for (const FAuditoryEvent& CurrentEvent : AuditoryEventQueue)
	{
		bool IsCombined {false};
		for (FAuditoryEvent& ExistingEvent : ProcessedEvents)
		{
			if (const float DistanceSquared {static_cast<float>(FVector::DistSquared(CurrentEvent.Location, ExistingEvent.Location))};
				DistanceSquared <= CombineRadiusSquared)
			{
				ExistingEvent = CombineAuditoryEvents(ExistingEvent, CurrentEvent);
				IsCombined = true;
				break;
			}
		}

		/** If the event was not merged with an already existing event, we add it to the array of combined events*/
		if (!IsCombined)
		{
			ProcessedEvents.Add(CurrentEvent);
		}
	}
	UE_LOG(LogSensoryEventManager, Verbose, TEXT("Combined '%d' auditory events to '%d' events."), AuditoryEventQueue.Num(), ProcessedEvents.Num())

	/** We empty the auditory event queue here as we only need the 'ProcessedEvents' array from this point forward. */
	AuditoryEventQueue.Empty();

	TArray<FHeatEvent> HeatEvents;
	
	for (FAuditoryEvent& AuditoryEvent : ProcessedEvents)
	{
		AttenuateLoudness(AuditoryEvent.Loudness, AuditoryEvent.Location, Nightstalker->GetActorLocation());

		if (AuditoryEvent.Loudness > 30)
		{
			HeatEvents.Add(FHeatEvent(LoudnessToHeat(AuditoryEvent.Loudness), GetHeatPointRadius(AuditoryEvent.Location, Nightstalker->GetActorLocation()), AuditoryEvent.Location));
		}
	}

	UE_LOG(LogSensoryEventManager, Verbose, TEXT("Attenuated auditory events to heat points: '%d'"), ProcessedEvents.Num())

	TArray<FHeatPointOverlapData> OverlapData;
	TArray<FHeatEvent> IsolatedHeatAtLocations;

	for (const FHeatEvent& HeatEvent : HeatEvents)
	{
		if (AHeatPoint* OverlappingHeatPoint {CheckForOverlaps(HeatEvent, Nightstalker->GetActorLocation(), GetWorld())})
		{
			FHeatPointOverlapData NewOverlapDataEntry {FHeatPointOverlapData(OverlappingHeatPoint, HeatEvent)};
			OverlapData.Add(NewOverlapDataEntry);
		}
		else
		{
			IsolatedHeatAtLocations.Add(HeatEvent);
		}
	}

	/** Update the existing heat points. */
	Director->GetHeatPointManager()->UpdateHeatPoints(OverlapData);

	HeatEvents = MoveTemp(IsolatedHeatAtLocations);

	if (HeatEvents.Num() == 0) { return; }
	
	TArray<FHeatEvent> ConsolidatedEvents = ConsolidateHeatEvents(HeatEvents, Nightstalker->GetActorLocation());
	
	for (const FHeatEvent& HeatEvent : ConsolidatedEvents)
	{
		if (AHeatPoint* NewHeatPoint {GetWorld()->SpawnActor<AHeatPoint>(AHeatPoint::StaticClass(), HeatEvent.Location, FRotator::ZeroRotator)})
		{
			NewHeatPoint->InitializeHeatPoint(HeatEvent.Radius, 60, HeatEvent.Heat);
			
			if (Director)
			{
				Director->RegisterHeatPoint(NewHeatPoint);
			}
		}
	}
}
