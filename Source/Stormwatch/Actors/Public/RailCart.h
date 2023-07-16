// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "RailCart.generated.h"

class ARailTrack;

UCLASS(Abstract, ClassGroup = "Railway", Meta = (DisplayName = "Rail Cart"))
class STORMWATCH_API ARailCart : public AActor
{
	GENERATED_BODY()

protected:
	/** The rail track actor that this rail cart will travel on. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (DisplayName = "Track"))
	TSoftObjectPtr<ARailCart> RailTrack {nullptr};
};
