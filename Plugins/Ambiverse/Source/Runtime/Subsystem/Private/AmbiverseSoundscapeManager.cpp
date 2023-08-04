// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseSoundscapeManager.h"
#include "AmbiverseSoundSourcePool.h"
#include "AmbiverseParameterManager.h"
#include "AmbiverseElement.h"
#include "AmbiverseSubsystem.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseSoundscapeManager, LogAmbiverseElementManager);

void UAmbiverseSoundscapeManager::RegisterElements(TArray<UAmbiverseElement*> Elements)
{
	if (Elements.IsEmpty()) { return; }
	
	for (UAmbiverseElement* Element : Elements)
	{
		if (Element)
		{
			ScheduleProceduralElement(Element, true);
			UE_LOG(LogAmbiverseElementManager, VeryVerbose, TEXT("Registered and scheduled element: '%s'"), *Element->GetName());
		}
	}
}

void UAmbiverseSoundscapeManager::UnregisterElements(TArray<UAmbiverseElement*> Elements)
{
	if (Elements.IsEmpty()) { return; }

	for (UAmbiverseElement* ProceduralElement : Elements)
	{
		ScheduledElementInstances.Remove(ProceduralElement);
	}

	for (UAmbiverseElement* ProceduralElement : Elements)
	{
		int32 i;
		PlayingProceduralElements.Find(ProceduralElement, i);

		PlayingProceduralElements[i]->IsPendingKill = true;
	}
}

void UAmbiverseSoundscapeManager::EvaluateFinishedElement(UAmbiverseElement* Element)
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

void UAmbiverseSoundscapeManager::Tick(const float DeltaTime)
{
	if (ScheduledElementInstances.IsEmpty()) { return; }
	
	for (UAmbiverseElement* ElementInstance : ScheduledElementInstances)
	{
		const float ScaleFactor {(ElementInstance->Time - DeltaTime) / ElementInstance->Time};
		ElementInstance->ReferenceTime *= ScaleFactor;
		
		ElementInstance->Time -= DeltaTime;

		if (ElementInstance->Time <= 0)
		{
			if (!Owner || !ElementInstance) { return; }
			
			if (UAmbiverseSoundSourcePool* SoundSourceManager {Owner->GetSoundSourcePool()})
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

void UAmbiverseSoundscapeManager::ScheduleProceduralElement(UAmbiverseElement* ElementInstance, const bool IgnoreMin)
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


