// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
// Written by Nino Saglia.

#pragma once

#include "CoreMinimal.h"
#include "ReacousticDataTypes.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPath.h"
#include "ReacousticSettings.generated.h"

UCLASS(config = Reacoustic, defaultconfig, Meta = (DisplayName = "Reacoustic"))
class REACOUSTIC_API UReacousticProjectSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()
private:


	
public:

	/** If there's a Reacoustic component correctly set up, it should enable generate hit events to the static mesh.
	 *There's no parent static mesh, all static meshes of the actor will generate hit events.
	 * If the component is childed to a specific static mesh component in the actor, it will listen to that static mesh only.
	 */
	UPROPERTY(Config, EditAnywhere)
	bool AutoEnableGenerateHitEvents{true};

	/** When drag and dropping a reacoustic sound onto an actor, should it automatically set the static mesh association?
	 *Otherwise it will add it to the actor instance in the level.
	 */
	UPROPERTY(Config, EditAnywhere)
	bool DragAndDropAssetSetsMeshAssociation{false};

	/** Override this if you want to add aditional logic to a child blueprint of ReacousticComponent */
	UPROPERTY(Config, EditAnywhere, Meta = (AllowedClasses = UReacousticComponent))
	FSoftObjectPath ReacousticComponent;

protected:



public:





#if WITH_EDITOR
	virtual FName GetCategoryName() const override { return FName(TEXT("Game")); }
	virtual FText GetSectionText() const override { return NSLOCTEXT("ReacousticPlugin", "ReacousticSettingsSection", "Reacoustic"); };

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
