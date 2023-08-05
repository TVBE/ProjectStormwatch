// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSceneAsset.h"
#include "AmbiverseSubsystemComponent.h"
#include "AmbiverseParameterManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParameterChangedDelegate, UAmbiverseParameterAsset*, ChangedParameter);

class UAmbiverseSceneAsset;
class UAmbiverseParameterAsset;

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
	TArray<UAmbiverseParameterAsset*> ParameterRegistry;

public:
	virtual void Initialize(UAmbiverseSubsystem* Subsystem) override;
	virtual void Deinitialize(UAmbiverseSubsystem* Subsystem) override;
	
	float GetParameterValue(UAmbiverseParameterAsset* Parameter);

	UFUNCTION(BlueprintCallable)
	void SetParameterValue(UAmbiverseParameterAsset* Parameter, const float Value);

private:
	void RegisterParameter(UAmbiverseParameterAsset* Parameter);
	
	bool IsParameterRegistered(const UAmbiverseParameterAsset* Parameter) const;

	UFUNCTION()
	void HandleOnLayerRegistered(UAmbiverseSceneAsset* RegisteredLayer);

public:
	/** Returns an array of currently registered parameters. */
	FORCEINLINE TArray<UAmbiverseParameterAsset*> GetRegisteredParameters() const { return ParameterRegistry; }
};
