// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseLayer.h"
#include "AmbiverseLayerManager.generated.h"

UCLASS()
class UAmbiverseLayerManager : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseLayerManager, Log, All)

private:
	/** The current active ambience layers. */
	UPROPERTY()
	TArray<UAmbiverseLayer*> LayerRegistry;

public:
	void AddAmbienceLayer(UAmbiverseLayer* Layer);
	void PopAmbienceLayer(UAmbiverseLayer* Layer);
	
	/** Checks if an ambience layer is already active*/
	UAmbiverseLayer* FindActiveAmbienceLayer(const UAmbiverseLayer* LayerToFind) const;

	FORCEINLINE TArray<UAmbiverseLayer*> GetLayerRegistry() const { return LayerRegistry; }
	
};
