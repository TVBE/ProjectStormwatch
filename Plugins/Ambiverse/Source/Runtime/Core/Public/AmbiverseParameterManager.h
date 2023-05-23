// Copyright 2023 Nino Saglia & Tim Verberne

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseLayer.h"
#include "AmbiverseParameterManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParameterChangedDelegate, UAmbiverseParameter*, ChangedParameter);

class UAmbiverseLayer;
class UAmbiverseParameter;

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Ambiverse")
class AMBIVERSE_API UAmbiverseParameterManager : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseParameterManager, Log, All)

public:
	UPROPERTY()
	FOnParameterChangedDelegate OnParameterChangedDelegate;

private:
	/** Pointer to the ambiverse subsystem. */
	UPROPERTY()
	UAmbiverseSubsystem* AmbiverseSubsystem {nullptr};
	
	/** The Ambiverse parameters that are currently registered to the system. */
	UPROPERTY()
	TArray<UAmbiverseParameter*> ParameterRegistry;

public:
	void Initialize(UAmbiverseSubsystem* Subsystem);
	void Deinitialize();
	
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
