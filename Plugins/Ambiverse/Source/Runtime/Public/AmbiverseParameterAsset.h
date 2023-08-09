// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseParameterAsset.generated.h"

/** An ambiverse scene is an instanced parameter that can be set through the ParameterManager in the AmbiverseSubsystem.
 *	Users can link modifiers to a parameter to change the density and volume of procedural sounds. */
UCLASS(Blueprintable, BlueprintType, ClassGroup = "Ambiverse", Meta = (DisplayName = "Ambiverse Parameter",
	ShortToolTip = "A parameter that can be used to drive procedural ambience."))
class AMBIVERSE_API UAmbiverseParameterAsset : public UObject
{
	GENERATED_BODY()
	
public:
	/** The value range of this parameter. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter", Meta = (DisplayName = "Value Range"))
	FVector2D ParameterRange {FVector2D(0, 100)};

	/** The default value of this parameter. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter", Meta = (DisplayName = "Default Value"))
	float DefaultValue {0.0f};

	/** The normalized scaled value of this parameter. */
	float ParameterValue;

public:
	float SetParameter(float Value);

private:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

USTRUCT(BlueprintType)
struct FAmbiverseParameterModifiers
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAmbiverseParameterAsset* Parameter {nullptr};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
	FVector2D DensityRange {FVector2D(1, 1)};
};
