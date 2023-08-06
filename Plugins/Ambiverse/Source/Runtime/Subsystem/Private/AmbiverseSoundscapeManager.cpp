// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseSoundscapeManager.h"
#include "AmbiverseLogCategories.h"
#include "AmbiverseSoundSourcePool.h"
#include "AmbiverseParameterManager.h"
#include "AmbiverseElement.h"
#include "AmbiverseElementAsset.h"
#include "AmbiverseSubsystem.h"

void UAmbiverseSoundscapeManager::RegisterElements(TArray<UAmbiverseElement*> Elements)
{
	if (Elements.IsEmpty()) { return; }
	
	for (UAmbiverseElement* Element : Elements)
	{
		if (Element)
		{
			ScheduleProceduralElement(Element, true);
			UE_LOG(LogAmbiverse, VeryVerbose, TEXT("Registered and scheduled element: '%s'"), *Element->GetName());
		}
	}
}

void UAmbiverseSoundscapeManager::UnregisterElements(TArray<UAmbiverseElement*> Elements)
{
	if (Elements.IsEmpty()) { return; }

	for (UAmbiverseElement* ProceduralElement : Elements)
	{
		ScheduledElements.Remove(ProceduralElement);
	}

	for (UAmbiverseElement* ProceduralElement : Elements)
	{
		int32 i;
		PlayingProceduralElements.Find(ProceduralElement, i);

		PlayingProceduralElements[i]->IsPendingKill = true;
	}
}

void UAmbiverseSoundscapeManager::PlayElement(const FAmbiverseElement& Element)
{
	if (!Element.IsValid()) { return; }

	FAmbiverseSoundSourceData SoundSourceData{FAmbiverseSoundSourceData()};

	SoundSourceData.Sound = Element.Asset->GetSound();
	SoundSourceData.Name = FName(Element.Asset->GetName());

	if(UAmbiverseDistributorPool* DistributionManager {Owner->GetDistributionManager()})
	{
		FTransform Transform {};
		if (DistributionManager->GetTransformForElement(Transform, ElementInstance))
		{
			SoundSourceData.Transform = Transform;
			
			ElementInstance->LastPlaylocation = Transform.GetLocation();
		}
		else
		{
			return;
		}
	}

	InitiateSoundSource(SoundSourceData, ElementInstance);

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
		UE_LOG(LogAmbiverse, VeryVerbose, TEXT("EvaluateFinishedElement: Destroyed element due to being marked PendingKill: '%s'"), *Element->GetName());
	}

	if (Element->RuntimeData.IntervalMode == EIntervalMode::OnSpawn && ScheduledElements.Contains(Element))
	{
		UE_LOG(LogAmbiverse, VeryVerbose, TEXT("EvaluateFinishedElement: Element is already scheduled: '%s'"), *Element->GetName());
		return;
	}
	if (Element->RuntimeData.IntervalMode == EIntervalMode::OnFinished)
	{
		ScheduleProceduralElement(Element);
		UE_LOG(LogAmbiverse, VeryVerbose, TEXT("EvaluateFinishedElement: Rescheduled element: '%s'"), *Element->GetName());
	}
}

UAmbiverseLayer* UAmbiverseSoundscapeManager::FindSceneByAsset(UAmbiverseSceneAsset* Asset) const
{
	for (UAmbiverseLayer* LayerInstance : Scenes)
	{
		if (LayerInstance && LayerInstance->Asset == Asset)
		{
			return LayerInstance;
		}
	}

	return nullptr;
}

void UAmbiverseSoundscapeManager::Initialize(UAmbiverseSubsystem* Subsystem)
{
	Super::Initialize(Subsystem);
	
	if (UAmbiverseParameterManager* ParameterManager {Subsystem->GetParameterManager()})
	{
		ParameterManager->OnParameterChangedDelegate.AddDynamic(this, &UAmbiverseSoundscapeManager::HandleOnParameterChanged);
	}
}

void UAmbiverseSoundscapeManager::Tick(const float DeltaTime)
{
	if (ScheduledElements.IsEmpty()) { return; }
	
	for (UAmbiverseElement* ElementInstance : ScheduledElements)
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
				UE_LOG(LogAmbiverse, VeryVerbose, TEXT("Tick: Playing element '%s'."), *ElementInstance->GetName());

				/** If the element has its intervalmode set to OnFinished, we remove the element from the array. */
				if (ElementInstance->RuntimeData.IntervalMode == EIntervalMode::OnFinished)
				{
					ScheduledElements.Remove(ElementInstance);
					return;
				}
			}
			
			ScheduleProceduralElement(ElementInstance); // THIS IS THE CULPRIT
			UE_LOG(LogAmbiverse, VeryVerbose, TEXT("Scheduled element '%s'."), *ElementInstance->GetName());
		}
	}
}

bool UAmbiverseSoundscapeManager::RegisterScene(UAmbiverseSceneAsset* LayerAsset)
{
	if (!LayerAsset)
	{
		UE_LOG(LogAmbiverse, Warning, TEXT("RegisterAmbiverseLayer: No Layer provided."));
		return false;
	}
	
	if (LayerAsset->Elements.IsEmpty())
	{
		UE_LOG(LogAmbiverse, Warning, TEXT("RegisterAmbiverseLayer: Layer has no procedural sounds: '%s'."),
			   *LayerAsset->GetName());
		return false;
	}
	
	if (!FindSceneByAsset(LayerAsset))
	{
		UAmbiverseLayer* LayerInstance {UAmbiverseLayer::CreateInstanceFromAsset(Owner, LayerAsset)};
		LayerInstance->Initialize(Owner);
		Scenes.Add(LayerInstance);
		
		OnLayerRegistered.Broadcast(LayerAsset);

		UE_LOG(LogAmbiverse, Verbose, TEXT("Registered Ambiverse Layer: '%s'."), *LayerAsset->GetName());

		return true;
		
	}
	return false;
}

bool UAmbiverseSoundscapeManager::UnregisterScene(UAmbiverseSceneAsset* LayerAsset, const bool ForceStop,
	const float FadeTime)
{
	if (!LayerAsset)
	{
		UE_LOG(LogAmbiverse, Warning, TEXT("UnregisterAmbiverseLayer: No LayerAsset provided."));
		return false;
	}

	if (UAmbiverseLayer* LayerInstance {FindSceneByAsset(LayerAsset)})
	{
		Scenes.Remove(LayerInstance);
		OnLayerUnregistered.Broadcast(LayerAsset);

		UE_LOG(LogAmbiverse, Verbose, TEXT("Unregistered Ambiverse Layer: '%s'."), *LayerAsset->GetName());
		return true;
	}
	return false;
}

void UAmbiverseSoundscapeManager::ScheduleProceduralElement(UAmbiverseElement* ElementInstance, const bool IgnoreMin)
{
	if (!ElementInstance || !Owner) { return; }
	
	float DensityScalar {1.0f};

	UAmbiverseSceneAsset* Layer {ElementInstance->AssociatedLayer};
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

	if (ScheduledElements.Contains(ElementInstance)) { return; }
	ScheduledElements.Add(ElementInstance);
}

void UAmbiverseSoundscapeManager::Deinitialize(UAmbiverseSubsystem* Subsystem)
{
	if (!Subsystem) { return; }
	
	if (UAmbiverseParameterManager* ParameterManager {Subsystem->GetParameterManager()})
	{
		ParameterManager->OnParameterChangedDelegate.RemoveDynamic(this, &UAmbiverseLayerManager::HandleOnParameterChanged);
	}
	
	Super::Deinitialize(Subsystem);
}


