// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseLayer.h"
#include "AmbiverseSubsystemComponent.h"
#include "AmbiverseParameterManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParameterChangedDelegate, UAmbiverseParameter*, ChangedParameter);

class UAmbiverseLayer;
class UAmbiverseParameter;

UCLASS()
class UAmbiverseParameterManager : public UAmbiverseSubsystemComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseParameterManager, Log, All)

public:
	UPROPERTY()
	FOnParameterChangedDelegate OnParameterChangedDelegate;

private:
	/** The Ambiverse parameters that are currently registered to the system. */
	UPROPERTY(Transient)
	TArray<UAmbiverseParameter*> ParameterRegistry;

public:
	virtual void Initialize(UAmbiverseSubsystem* Subsystem) override;
	virtual void Deinitialize(UAmbiverseSubsystem* Subsystem) override;
	
	void GetScalarsForEntry(float& DensityScalar, float& VolumeScalar, const UAmbiverseLayer* Layer, const FAmbiverseLayerQueueEntry& Entry);

	UFUNCTION(BlueprintCallable)
	void SetParameterValue(UAmbiverseParameter* Parameter, const float Value);

private:
	void RegisterParameter(UAmbiverseParameter* Parameter);
	
	bool IsParameterRegistered(const UAmbiverseParameter* Parameter) const;

	UFUNCTION()
	void HandleOnLayerRegistered(UAmbiverseLayer* RegisteredLayer);

public:
	/** Returns an array of currently registered parameters. */
	FORCEINLINE TArray<UAmbiverseParameter*> GetRegisteredParameters() const { return ParameterRegistry; }
};
