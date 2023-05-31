// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AmbiverseDistributor.generated.h"


UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Ambiverse", Meta = (DisplayName = "Ambiverse Distributor",
	ShortToolTip = "A distributor allows for blueprint scripting of custom spawn behavior."))
class AMBIVERSE_API UAmbiverseDistributor : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY(Transient)
	AActor* Listener;

public:
	void Activate(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintImplementableEvent, Meta = (WorldContext = "WorldContextObject"))
	bool ExecuteDistribution(UObject* WorldContextObject, FVector& Location, AActor*& Actor);

protected:
	UFUNCTION(BlueprintPure, Meta = (CompactNodeTitle = "Listener"))
	FORCEINLINE AActor* GetListener() { return Listener; }
};
