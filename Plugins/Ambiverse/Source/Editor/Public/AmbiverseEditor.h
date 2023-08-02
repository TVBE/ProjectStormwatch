// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_AmbiverseComposite.h"
#include "AssetTypeActions_AmbiverseElement.h"
#include "AssetTypeActions_AmbiverseLayer.h"
#include "AssetTypeActions_AmbiverseParameter.h"
#include "Modules/ModuleManager.h"

class FAmbiverseEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FAssetTypeActions_AmbiverseElement> AmbiverseElementAssetTypeActions;
	TSharedPtr<FAssetTypeActions_AmbiverseLayer> AmbiverseLayerAssetTypeActions;
	TSharedPtr<FAssetTypeActions_AmbiverseComposite> AmbiverseCompositeAssetTypeActions;
	TSharedPtr<FAssetTypeActions_AmbiverseParameter> AmbiverseParameterAssetTypeActions;
};

/** Asset macros */
#define AMBIVERSE_ASSET_COLOR FColor(210, 20, 70)
#define AMBIVERSE_ASSET_CATEGORY EAssetTypeCategories::Sounds
#define AMBIVERSE_ASSET_SUBMENUS LOCTEXT("AssetAmbiverseSubMenu", "Ambiverse")
