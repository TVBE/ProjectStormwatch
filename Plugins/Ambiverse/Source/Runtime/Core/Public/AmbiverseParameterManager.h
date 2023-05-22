// Copyright 2023 Nino Saglia & Tim Verberne

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AmbiverseParameterManager.generated.h"

class UAmbiverseParameter;

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Ambiverse")
class AMBIVERSE_API UAmbiverseParameterManager : public UObject
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogAmbiverseParameterManager, Log, All)

private:
	/** The Ambiverse parameters that are currently registered to the system. */
	UPROPERTY()
	TArray<UAmbiverseParameter*> ParameterRegistry;

public:
	

private:
	void RegisterLayer(UAmbiverseParameter* Parameter);
	
	bool IsParameterRegistered(const UAmbiverseParameter* Parameter) const;

public:
	/** Returns an array of currently registered parameters. */
	FORCEINLINE TArray<UAmbiverseParameter*> GetRegisteredParameters() const { return ParameterRegistry; }
};
