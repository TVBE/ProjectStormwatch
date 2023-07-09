// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseCompositeAsset.generated.h"

class UAmbiverseLayerAsset;

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Ambiverse", Meta = (DisplayName = "Ambiverse Composite",
	ShortToolTip = "Collection of Ambiverse Layers that can be activated at once."))
class AMBIVERSE_API UAmbiverseCompositeAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Layers", Meta = (TitleProperty = "{Name}"))
	TArray<UAmbiverseLayerAsset*> Layers;

	/** If true, all layers that are not part of this composite are popped when this composite is pushed. */
	UPROPERTY(EditAnywhere, Category = "Settings", Meta = (DisplayName = "Stop Non-Composite layers"))
	bool StopNonCompositeLayers {false};
};
