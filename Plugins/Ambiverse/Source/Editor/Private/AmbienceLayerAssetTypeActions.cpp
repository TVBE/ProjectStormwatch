// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseLayerAssetTypeActions.h"
#include "AmbiverseLayerAsset.h"
#include "AmbiverseEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAmbiverseLayerAssetTypeActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_AmbiverseLayer", "Ambiverse Layer");
}

UClass* FAmbiverseLayerAssetTypeActions::GetSupportedClass() const
{
	return UAmbiverseLayerAsset::StaticClass();
}

FColor FAmbiverseLayerAssetTypeActions::GetTypeColor() const
{
	return AMBIVERSE_ASSET_COLOR;
}

uint32 FAmbiverseLayerAssetTypeActions::GetCategories()
{
	return AMBIVERSE_ASSET_CATEGORY;
}

const TArray<FText>& FAmbiverseLayerAssetTypeActions::GetSubMenus() const
{
	static const TArray<FText> SubMenus
{
	AMBIVERSE_ASSET_SUBMENUS
};
	return SubMenus;
}
