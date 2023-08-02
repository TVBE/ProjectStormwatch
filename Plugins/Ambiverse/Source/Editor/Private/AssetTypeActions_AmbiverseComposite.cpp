// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AssetTypeActions_AmbiverseComposite.h"
#include "AmbiverseCompositeAsset.h"
#include "AmbiverseEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_AmbiverseComposite::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_AmbiverseComposite", "Ambiverse Composite");
}

UClass* FAssetTypeActions_AmbiverseComposite::GetSupportedClass() const
{
	return UAmbiverseCompositeAsset::StaticClass();
}

FColor FAssetTypeActions_AmbiverseComposite::GetTypeColor() const
{
	return AMBIVERSE_ASSET_COLOR;
}

uint32 FAssetTypeActions_AmbiverseComposite::GetCategories()
{
	return AMBIVERSE_ASSET_CATEGORY;
}

const TArray<FText>& FAssetTypeActions_AmbiverseComposite::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		AMBIVERSE_ASSET_SUBMENUS
	};

	return SubMenus;
}