// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHStormwatchGameMode.h"
#include "BHPlayerCharacter.h"

// ---------------------------------------------------------------------------------------------------------------------
// ************************************************* Editor Utilities **************************************************
// ---------------------------------------------------------------------------------------------------------------------

ABHStormwatchGameMode::ABHStormwatchGameMode()
{
#if WITH_EDITOR
	static const FName PropertyEditor("PropertyEditor");
	FPropertyEditorModule& PropertyModule {FModuleManager::GetModuleChecked<FPropertyEditorModule>(PropertyEditor)};

#define LOCTEXT_NAMESPACE "PropertySection"
	const TSharedRef<FPropertySection> Door
	{PropertyModule.FindOrCreateSection("SlidingDoor", "Door", LOCTEXT("Door", "Door"))};
	Door->AddCategory("Door");
	
	const TSharedRef<FPropertySection> Button
	{PropertyModule.FindOrCreateSection("PressableButton", "Button", LOCTEXT("Button", "Button"))};
	Button->AddCategory("Button");

	const TSharedRef<FPropertySection> Sensor
		{PropertyModule.FindOrCreateSection("ProximitySensor", "Sensor", LOCTEXT("Sensor", "Sensor"))};
	Sensor->AddCategory("Sensor");
#undef LOCTEXT_NAMESPACE
	
#endif
}

// ---------------------------------------------------------------------------------------------------------------------
// *********************************************************************************************************************
// ---------------------------------------------------------------------------------------------------------------------


void ABHStormwatchGameMode::NotifyPlayerCharacterBeginPlay(ABHPlayerCharacter* Character)
{
	if (Character)
	{
		IsPlayerActive = true;
		EventOnPlayerSpawn(Character);
	}
}

void ABHStormwatchGameMode::EventOnPlayerSpawn_Implementation(ABHPlayerCharacter* Character)
{
}

