// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AssetTypeActions_AmbiverseLayer.h"
#include "AmbiverseLayerAsset.h"
#include "AmbiverseEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_AmbiverseLayer::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_AmbiverseLayer", "Ambiverse Layer");
}

UClass* FAssetTypeActions_AmbiverseLayer::GetSupportedClass() const
{
	return UAmbiverseLayerAsset::StaticClass();
}

FColor FAssetTypeActions_AmbiverseLayer::GetTypeColor() const
{
	return AMBIVERSE_ASSET_COLOR;
}

uint32 FAssetTypeActions_AmbiverseLayer::GetCategories()
{
	return AMBIVERSE_ASSET_CATEGORY;
}

const TArray<FText>& FAssetTypeActions_AmbiverseLayer::GetSubMenus() const
{
	static const TArray<FText> SubMenus
{
	AMBIVERSE_ASSET_SUBMENUS
};
	return SubMenus;
}
