// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SubsystemComponent.h"
#include "DebugComponent.generated.h"

UCLASS()
class AMBIVERSE_API UDebugComponent : public USubsystemComponent
{
	GENERATED_BODY()

public:
	TUniquePtr<FAutoConsoleCommand> SoundSourceVisualisationConsoleCommand;

	bool IsDebugEnabled {true};

public:
	virtual void Initialize(UAmbiverseSubsystem* Subsystem) override;
	
	virtual void Tick(const float DeltaTime) override;
	
	void SetSoundSourceVisualisationEnabled(bool IsEnabled);
};
