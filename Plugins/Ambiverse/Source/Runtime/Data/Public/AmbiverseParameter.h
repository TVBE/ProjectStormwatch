// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseParameter.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Ambiverse")
class AMBIVERSE_API UAmbiverseParameter : public UObject
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
	UAmbiverseParameter* Parameter {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "2", UIMin = "0", UIMax = "2"))
	FVector2D VolumeRange {FVector2D(1, 1)};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = "0", ClampMax = "10", UIMin = "0", UIMax = "10"))
	FVector2D DensityRange {FVector2D(1, 1)};
};
