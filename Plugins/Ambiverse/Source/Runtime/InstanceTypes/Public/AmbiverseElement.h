// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseLayerAsset.h"
#include "AmbiverseElementRuntimeData.h"
#include "AmbiverseElement.generated.h"

class UAmbiverseSubsystem;
class AAmbiverseSoundSource;

UCLASS(MinimalAPI, Transient)
class UAmbiverseElement : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseProceduralElement, Log, All)

public:
	UPROPERTY()
	FAmbiverseElementRuntimeData RuntimeData;
	
	bool IsPlaying {false};
	bool IsPendingKill {false};

	/** The Ambiverse layer this element is associated with. This can be a nullptr if the element was instanced without a layer. this is rare though. */
	UPROPERTY()
	UAmbiverseLayerAsset* AssociatedLayer;

	/** The last location this element was played at. We only really use this if the distribution mode is set to static.
	 *	We save this vector here independently from the SoundSource used to play this element, as the SoundSource might have been re-used for another element before
	 *	we need this data. */
	FVector LastPlaylocation;
	
	float Time {0.0f};
	
	/** The reference time the entry was initialized with.
	 *	We use this time value to be able to dynamically apply parameters in real time without breaking the existing schedule. */
	float ReferenceTime {0.0f};
	
	void Initialize(const FAmbiverseElementRuntimeData& InProceduralElementData, UAmbiverseLayerAsset* Layer)
	{
		RuntimeData = InProceduralElementData;
		AssociatedLayer = Layer;
	}
	
	static UAmbiverseElement* CreateInstanceFromRuntimeData(UObject* Outer, FAmbiverseElementRuntimeData& RuntimeData);
};
