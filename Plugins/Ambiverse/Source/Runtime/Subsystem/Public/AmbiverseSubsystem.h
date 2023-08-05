// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AmbiverseSubsystem.generated.h"

class UAmbiverseSubsystemComponent;
class UAmbiverseSoundscapeManager;
class UAmbiverseElement;
class UAmbiverseDebugVisualizationComponent;
class UAmbiverseLayerManager;
class UAmbiverseParameterManager;
class UAmbiverseSoundSourcePool;
class UAmbiverseSceneAsset;

UCLASS(Transient, ClassGroup = "Ambiverse", Meta = (DisplayName = "Ambiverse"))
class AMBIVERSE_API UAmbiverseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	/** Array containing all subsystem components in this subsystem.
	 *	@note The elements in this array can be garbage collected when the
	 *	strong object pointers are reset or go out of scope. */
	TArray<UAmbiverseSubsystemComponent*> SubsystemComponents;

	TStrongObjectPtr<UAmbiverseSoundscapeManager> SoundscapeManager;
	TStrongObjectPtr<UAmbiverseParameterManager> ParameterManager;
	TStrongObjectPtr<UAmbiverseSoundSourcePool> SoundSourcePool;

#if !UE_BUILD_SHIPPING
	TStrongObjectPtr<UAmbiverseDebugVisualizationComponent> DebugComponent;
#endif

public:
	UAmbiverseSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	FORCEINLINE UAmbiverseSoundscapeManager* GetSoundscapeManager() const { return SoundscapeManager.Get(); }
	FORCEINLINE UAmbiverseParameterManager* GetParameterManager() const { return ParameterManager.Get(); }
	FORCEINLINE UAmbiverseSoundSourcePool* GetSoundSourcePool() const { return SoundSourcePool.Get(); }
};



