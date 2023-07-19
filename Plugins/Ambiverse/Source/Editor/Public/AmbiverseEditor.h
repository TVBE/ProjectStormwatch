// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseCompositeAssetTypeActions.h"
#include "AmbiverseElementAssetTypeActions.h"
#include "AmbiverseLayerAssetTypeActions.h"
#include "AmbiverseParameterAssetTypeActions.h"
#include "Modules/ModuleManager.h"

class FAmbiverseEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FAmbiverseElementAssetTypeActions> AmbiverseElementAssetTypeActions;
	TSharedPtr<FAmbiverseLayerAssetTypeActions> AmbiverseLayerAssetTypeActions;
	TSharedPtr<FAmbiverseCompositeAssetTypeActions> AmbiverseCompositeAssetTypeActions;
	TSharedPtr<FAmbiverseParameterAssetTypeActions> AmbiverseParameterAssetTypeActions;
};

/** Asset macros */
#define AMBIVERSE_ASSET_COLOR FColor(210, 20, 70)
#define AMBIVERSE_ASSET_CATEGORY EAssetTypeCategories::Sounds
#define AMBIVERSE_ASSET_SUBMENUS LOCTEXT("AssetAmbiverseSubMenu", "Ambiverse")
