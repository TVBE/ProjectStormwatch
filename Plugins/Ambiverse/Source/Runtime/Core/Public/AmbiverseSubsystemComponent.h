// Copyright (c) 2023-present Tim Verberne
// This source code is part of the Ambiverse plugin

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AmbiverseSubsystemComponent.generated.h"

class UAmbiverseSubsystem;

UCLASS(MinimalAPI, Transient, Within = "AmbiverseSubsystem", ClassGroup = "AmbiVerse")
class UAmbiverseSubsystemComponent : public UObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	UAmbiverseSubsystem* Owner;

public:
	virtual void Initialize(UAmbiverseSubsystem* Subsystem);
	virtual void Deinitialize(UAmbiverseSubsystem* Subsystem);
	
	FORCEINLINE UAmbiverseSubsystem* GetOwner() const { return Owner; }
};
