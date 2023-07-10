// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseSubsystem.h"
#include "UObject/NoExportTypes.h"
#include "AmbiverseSubsystemComponent.generated.h"

class UAmbiverseSubsystem;

UCLASS(MinimalAPI, Transient, Within = "AmbiverseSubsystem", ClassGroup = "AmbiVerse")
class UAmbiverseSubsystemComponent : public UObject
{
	GENERATED_BODY()

public:
	bool IsInitialized {false};
	
protected:
	UPROPERTY()
	UAmbiverseSubsystem* Owner;

public:
	virtual void Initialize(UAmbiverseSubsystem* Subsystem);
	virtual void Deinitialize(UAmbiverseSubsystem* Subsystem);
	
	virtual void Tick(const float DeltaTime) {};

	virtual UWorld* GetWorld() const override
	{
		if (Owner) { return Owner->GetWorld(); }
		return nullptr;
	}
	
	FORCEINLINE UAmbiverseSubsystem* GetOwner() const { return Owner; }
};
