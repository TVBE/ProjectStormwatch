// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AmbiverseSubsystem.generated.h"

class USubsystemComponent;
class USoundscapeManager;
class UAmbiverseElement;
class UDebugComponent;
class UAmbiverseLayerManager;
class UParameterManager;
class USoundSourcePool;
class UAmbiverseSceneAsset;

UCLASS(Transient, ClassGroup = "Ambiverse", Meta = (DisplayName = "Ambiverse"))
class AMBIVERSE_API UAmbiverseSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	/** Array containing all subsystem components in this subsystem.
	 *	@note The elements in this array can be garbage collected when the
	 *	strong object pointers are reset or go out of scope. */
	TArray<USubsystemComponent*> SubsystemComponents;

	TStrongObjectPtr<USoundscapeManager> SoundscapeManager;
	TStrongObjectPtr<UParameterManager> ParameterManager;
	TStrongObjectPtr<USoundSourcePool> SoundSourcePool;

#if !UE_BUILD_SHIPPING
	TStrongObjectPtr<UDebugComponent> DebugComponent;
#endif

public:
	UAmbiverseSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	FORCEINLINE USoundscapeManager* GetSoundscapeManager() const { return SoundscapeManager.Get(); }
	FORCEINLINE UParameterManager* GetParameterManager() const { return ParameterManager.Get(); }
	FORCEINLINE USoundSourcePool* GetSoundSourcePool() const { return SoundSourcePool.Get(); }
};



