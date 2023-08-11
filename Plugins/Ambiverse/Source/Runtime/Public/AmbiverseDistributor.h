// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseDistributor.generated.h"

class UAmbiverseElementAsset;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Ambiverse", Meta = (DisplayName = "Ambiverse Distributor",
	ShortToolTip = "A distributor allows for blueprint scripting of custom spawn behavior."))
class AMBIVERSE_API UAmbiverseDistributor : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseDistributor, Log, All)

private:
	UPROPERTY(Transient)
	UWorld* World;

public:
	void Initialize(const UObject* WorldContextObject);
	
	virtual UWorld* GetWorld() const override final { return World; };
	
	UFUNCTION(BlueprintImplementableEvent, Meta = (WorldContext = "WorldContextObject"))
	bool ExecuteDistribution(FTransform& Transform, const FVector& ListenerLocation, const UAmbiverseElementAsset* Element);
};

