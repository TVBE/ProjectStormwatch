// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReacousticSoundAssetTypeActions.h"
#include "Modules/ModuleManager.h"

class FReacousticEditorModule: public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FReacousticSoundAssetTypeActions> ReacousticSoundAssetTypeActions;

#define REACOUSTIC_ASSET_COLOR FColor(0, 255, 130)
	
};