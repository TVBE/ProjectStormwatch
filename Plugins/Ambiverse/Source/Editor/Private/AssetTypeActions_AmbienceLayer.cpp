// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AssetTypeActions_AmbiverseScene.h"
#include "AmbiverseSceneAsset.h"
#include "AmbiverseEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_AmbiverseScene::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_AmbiverseLayer", "Ambiverse Layer");
}

UClass* FAssetTypeActions_AmbiverseScene::GetSupportedClass() const
{
	return UAmbiverseSceneAsset::StaticClass();
}

FColor FAssetTypeActions_AmbiverseScene::GetTypeColor() const
{
	return AMBIVERSE_ASSET_COLOR;
}

uint32 FAssetTypeActions_AmbiverseScene::GetCategories()
{
	return AMBIVERSE_ASSET_CATEGORY;
}

const TArray<FText>& FAssetTypeActions_AmbiverseScene::GetSubMenus() const
{
	static const TArray<FText> SubMenus
{
	AMBIVERSE_ASSET_SUBMENUS
};
	return SubMenus;
}
