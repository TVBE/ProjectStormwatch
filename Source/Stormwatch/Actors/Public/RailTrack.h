// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "RailTrack.generated.h"

class USplineComponent;

UCLASS(Abstract, ClassGroup = "Railway", Meta = (DisplayName = "Rail Track"))
class STORMWATCH_API ARailTrack : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USplineComponent* SplineComponent {nullptr};

public:
	ARailTrack();

public:
	/** Returns the spline component of this rail track. */
	FORCEINLINE USplineComponent* GetSplineComponent() const { return SplineComponent; }
};
