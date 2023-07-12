// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin.

#include "AmbiverseCompositeAssetTypeActions.h"
#include "AmbiverseCompositeAsset.h"
#include "AmbiverseEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAmbiverseCompositeAssetTypeActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_AmbiverseComposite", "Ambiverse Composite");
}

UClass* FAmbiverseCompositeAssetTypeActions::GetSupportedClass() const
{
	return UAmbiverseCompositeAsset::StaticClass();
}

FColor FAmbiverseCompositeAssetTypeActions::GetTypeColor() const
{
	return AMBIVERSE_ASSET_COLOR;
}

uint32 FAmbiverseCompositeAssetTypeActions::GetCategories()
{
	return AMBIVERSE_ASSET_CATEGORY;
}

const TArray<FText>& FAmbiverseCompositeAssetTypeActions::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		AMBIVERSE_ASSET_SUBMENUS
	};

	return SubMenus;
}