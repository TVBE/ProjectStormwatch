// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseElementManager.h"
#include "AmbiverseSoundSourceManager.h"
#include "AmbiverseParameterManager.h"
#include "AmbiverseElementInstance.h"
#include "AmbiverseSubsystem.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseElementManager, LogAmbiverseElementManager);

void UAmbiverseElementManager::RegisterElements(TArray<UAmbiverseElementInstance*> Elements)
{
	if (Elements.IsEmpty()) { return; }
	
	for (UAmbiverseElementInstance* Element : Elements)
	{
		if (Element)
		{
			ScheduleProceduralElement(Element, true);
			UE_LOG(LogAmbiverseElementManager, VeryVerbose, TEXT("Registered and scheduled element: '%s'"), *Element->GetName());
		}
	}
}

void UAmbiverseElementManager::UnregisterElements(TArray<UAmbiverseElementInstance*> Elements)
{
	if (Elements.IsEmpty()) { return; }

	for (UAmbiverseElementInstance* ProceduralElement : Elements)
	{
		ScheduledElementInstances.Remove(ProceduralElement);
	}

	for (UAmbiverseElementInstance* ProceduralElement : Elements)
	{
		int32 i;
		PlayingProceduralElements.Find(ProceduralElement, i);

		PlayingProceduralElements[i]->IsPendingKill = true;
	}
}

void UAmbiverseElementManager::EvaluateFinishedElement(UAmbiverseElementInstance* Element)
{
	if (!Element)
	{
		UE_LOG(LogTemp, Error, TEXT("Element is null"))
		return;
	}

	/** If the element was marked pending kill while being associated with an sound source, we destroy the element at this moment. */
	if (Element->IsPendingKill)
	{
		Element->ConditionalBeginDestroy();
		UE_LOG(LogAmbiverseElementManager, VeryVerbose, TEXT("EvaluateFinishedElement: Destroyed element due to being marked PendingKill: '%s'"), *Element->GetName());
	}

	if (Element->RuntimeData.IntervalMode == EIntervalMode::OnSpawn && ScheduledElementInstances.Contains(Element))
	{
		UE_LOG(LogAmbiverseElementManager, VeryVerbose, TEXT("EvaluateFinishedElement: Element is already scheduled: '%s'"), *Element->GetName());
		return;
	}
	if (Element->RuntimeData.IntervalMode == EIntervalMode::OnFinished)
	{
		ScheduleProceduralElement(Element);
		UE_LOG(LogAmbiverseElementManager, VeryVerbose, TEXT("EvaluateFinishedElement: Rescheduled element: '%s'"), *Element->GetName());
	}
}

void UAmbiverseElementManager::Tick(const float DeltaTime)
{
	if (ScheduledElementInstances.IsEmpty()) { return; }
	
	for (UAmbiverseElementInstance* ElementInstance : ScheduledElementInstances)
	{
		const float ScaleFactor {(ElementInstance->Time - DeltaTime) / ElementInstance->Time};
		ElementInstance->ReferenceTime *= ScaleFactor;
		
		ElementInstance->Time -= DeltaTime;

		if (ElementInstance->Time <= 0)
		{
			if (!Owner || !ElementInstance) { return; }
			
			if (UAmbiverseSoundSourceManager* SoundSourceManager {Owner->GetSoundSourceManager()})
			{
				SoundSourceManager->PlayElement(ElementInstance);
				UE_LOG(LogAmbiverseElementManager, VeryVerbose, TEXT("Tick: Playing element '%s'."), *ElementInstance->GetName());

				/** If the element has its intervalmode set to OnFinished, we remove the element from the array. */
				if (ElementInstance->RuntimeData.IntervalMode == EIntervalMode::OnFinished)
				{
					ScheduledElementInstances.Remove(ElementInstance);
					return;
				}
			}
			
			ScheduleProceduralElement(ElementInstance); // THIS IS THE CULPRIT
			UE_LOG(LogAmbiverseElementManager, VeryVerbose, TEXT("Scheduled element '%s'."), *ElementInstance->GetName());
		}
	}
}

void UAmbiverseElementManager::ScheduleProceduralElement(UAmbiverseElementInstance* ElementInstance, const bool IgnoreMin)
{
	if (!ElementInstance || !Owner) { return; }
	
	float DensityScalar {1.0f};

	UAmbiverseLayerAsset* Layer {ElementInstance->AssociatedLayer};
	UAmbiverseParameterManager* ParameterManager {Owner->GetParameterManager()};
		
	if(Layer && ParameterManager)
	{
		if (TArray<FAmbiverseParameterModifiers> Modifiers {Layer->Parameters}; !Modifiers.IsEmpty())
		{
			for (FAmbiverseParameterModifiers& ParameterModifiers : Modifiers)
			{
				DensityScalar = 1 / FMath::GetMappedRangeValueClamped(FVector2D(0, 1), ParameterModifiers.DensityRange,
					ParameterManager->GetParameterValue(ParameterModifiers.Parameter));
				DensityScalar = ParameterManager->GetParameterValue(ParameterModifiers.Parameter);
			}
		}
	}
	const double MinimumValue {IgnoreMin ? 0 : ElementInstance->RuntimeData.IntervalRange.X};
	
	ElementInstance->ReferenceTime = FMath::RandRange(MinimumValue,ElementInstance->RuntimeData.IntervalRange.Y);
	ElementInstance->Time = ElementInstance->ReferenceTime * DensityScalar;

	if (ScheduledElementInstances.Contains(ElementInstance)) { return; }
	ScheduledElementInstances.Add(ElementInstance);
}


