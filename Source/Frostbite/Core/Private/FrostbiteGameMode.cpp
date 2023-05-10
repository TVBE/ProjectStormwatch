// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "FrostbiteGameMode.h"
#include "PlayerCharacter.h"

// ---------------------------------------------------------------------------------------------------------------------
// ************************************************* Editor Utilities **************************************************
// ---------------------------------------------------------------------------------------------------------------------

AFrostbiteGameMode::AFrostbiteGameMode()
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
#undef LOCTEXT_NAMESPACE
	
#endif
}

// ---------------------------------------------------------------------------------------------------------------------
// *********************************************************************************************************************
// ---------------------------------------------------------------------------------------------------------------------


void AFrostbiteGameMode::NotifyPlayerCharacterBeginPlay(APlayerCharacter* Character)
{
	if (Character)
	{
		IsPlayerActive = true;
		EventOnPlayerSpawn(Character);
	}
}

void AFrostbiteGameMode::EventOnPlayerSpawn_Implementation(APlayerCharacter* Character)
{
}

