// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseParameterAssetTypeActions.h"
#include "AmbiverseParameterAsset.h"
#include "AmbiverseEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAmbiverseParameterAssetTypeActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_AmbiverseLayer", "Ambiverse Parameter");
}

UClass* FAmbiverseParameterAssetTypeActions::GetSupportedClass() const
{
	return UAmbiverseParameterAsset::StaticClass();
}

FColor FAmbiverseParameterAssetTypeActions::GetTypeColor() const
{
	return AMBIVERSE_ASSET_COLOR;
}

uint32 FAmbiverseParameterAssetTypeActions::GetCategories()
{
	return AMBIVERSE_ASSET_CATEGORY;
}

const TArray<FText>& FAmbiverseParameterAssetTypeActions::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		AMBIVERSE_ASSET_SUBMENUS
	};

	return SubMenus;
}
