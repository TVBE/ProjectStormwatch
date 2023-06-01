// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AmbiverseSubsystem.generated.h"

class UAmbiverseDistributorManager;
class UAmbiverseLayerManager;
class UAmbiverseParameterManager;
class UAmbiverseSoundSourceManager;
class UAmbiverseLayer;

UCLASS(Transient, ClassGroup = "Ambiverse")
class AMBIVERSE_API UAmbiverseSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseSubsystem, Log, All)

public:
#if !UE_BUILD_SHIPPING
	TUniquePtr<FAutoConsoleCommand> SoundSourceVisualisationConsoleCommand;
#endif

private:
	/** Sub objects. */
	UPROPERTY()
	UAmbiverseLayerManager* LayerManager {nullptr};
	
	UPROPERTY()
	UAmbiverseParameterManager* ParameterManager {nullptr};

	UPROPERTY()
	UAmbiverseSoundSourceManager* SoundSourceManager {nullptr};

	UPROPERTY()
	UAmbiverseDistributorManager* DistributorManager {nullptr};

public:
	UAmbiverseSubsystem();
	
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UAmbiverseSubsystem, STATGROUP_Tickables);
	}

	/** Processes an ambience event and updates the queue for an ambience layer. */
	UFUNCTION()
	void ProcessProceduralElement(UAmbiverseLayer* Layer,  FAmbiverseProceduralElement& ProceduralElement);

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	virtual void Tick(float DeltaTime) override;
	
	static float GetSoundInterval(const UAmbiverseLayer* Layer, const FAmbiverseProceduralElement& ProceduralElement);
	static float GetSoundVolume(const UAmbiverseLayer* Layer, const FAmbiverseProceduralElement& ProceduralElement);

	UFUNCTION()
	void HandleParameterChanged();

#if !UE_BUILD_SHIPPING
	void SetSoundSourceVisualisationEnabled(bool IsEnabled);
#endif

public:
	FORCEINLINE UAmbiverseLayerManager* GetLayerManager() const { return LayerManager; }
	FORCEINLINE UAmbiverseParameterManager* GetParameterManager() const { return ParameterManager; }
	FORCEINLINE UAmbiverseSoundSourceManager* GetSoundSourceManager() const { return SoundSourceManager; }
	FORCEINLINE UAmbiverseDistributorManager* GetDistributorManager() const { return DistributorManager; }
};



