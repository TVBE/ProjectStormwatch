// Copyright 2023 Nino Saglia & Tim Verberne

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPath.h"
#include "ReacousticSettings.generated.h"

/**
 * 
 */
UCLASS(config = Reacoustic, defaultconfig, Meta = (DisplayName = "Reacoustic"))
class REACOUSTIC_API UReacousticProjectSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Meta = (AllowedClasses = DataTable))
	FSoftObjectPath ReacousticObjectSounds;

	UPROPERTY(Config, EditAnywhere, Meta = (AllowedClasses = DataTable))
	FSoftObjectPath ReacousticSurfaceSounds;

public:

	// Beginning of UDeveloperSettings Interface
	virtual FName GetCategoryName() const override { return FName(TEXT("Game")); }
#if WITH_EDITOR
	virtual FText GetSectionText() const override { return NSLOCTEXT("ReacousticPlugin", "ReacousticSettingsSection", "Reacoustic"); };
#endif
	// End of UDeveloperSettings Interface

	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
