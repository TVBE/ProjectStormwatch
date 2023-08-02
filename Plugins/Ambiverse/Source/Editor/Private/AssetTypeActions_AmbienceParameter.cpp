// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AssetTypeActions_AmbiverseParameter.h"
#include "AmbiverseParameterAsset.h"
#include "AmbiverseEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_AmbiverseParameter::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_AmbiverseLayer", "Ambiverse Parameter");
}

UClass* FAssetTypeActions_AmbiverseParameter::GetSupportedClass() const
{
	return UAmbiverseParameterAsset::StaticClass();
}

FColor FAssetTypeActions_AmbiverseParameter::GetTypeColor() const
{
	return AMBIVERSE_ASSET_COLOR;
}

uint32 FAssetTypeActions_AmbiverseParameter::GetCategories()
{
	return AMBIVERSE_ASSET_CATEGORY;
}

const TArray<FText>& FAssetTypeActions_AmbiverseParameter::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		AMBIVERSE_ASSET_SUBMENUS
	};

	return SubMenus;
}
