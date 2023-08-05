// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSubsystem.h"
#include "AmbiverseSubsystemComponent.generated.h"

class UAmbiverseSubsystem;

UCLASS(Abstract, MinimalAPI, Transient, Within = "AmbiverseSubsystem", ClassGroup = "Ambiverse")
class UAmbiverseSubsystemComponent : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	UAmbiverseSubsystem* Owner;

	bool IsTickEnabled {false};

private:
	bool IsComponentInitialized {false};

public:
	virtual void Initialize(UAmbiverseSubsystem* Subsystem);
	virtual void Deinitialize(UAmbiverseSubsystem* Subsystem);
	
	virtual UWorld* GetWorld() const override final
	{
		if (Owner) { return Owner->GetWorld(); }
		return nullptr;
	}

	/** FTickableGameObject implementation begin. */
	virtual UWorld* GetTickableGameObjectWorld() const override final { return GetWorld(); }
	virtual ETickableTickType GetTickableTickType() const override final { return ETickableTickType::Conditional; }
	virtual bool IsAllowedToTick() const override final { return !IsTemplate() && IsComponentInitialized; }
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UAmbiverseSubsystemComponent, STATGROUP_Tickables); }
	/** FTickableGameObject implementation end. */
	
	FORCEINLINE UAmbiverseSubsystem* GetOwner() const { return Owner; }

	FORCEINLINE bool CanTick() const { return IsComponentInitialized && IsTickEnabled;}

	FORCEINLINE bool IsInitialized() const { return IsComponentInitialized; }
};
