// Copyright (c) 2022-present Tim Verberne
// This source code is part of the Adaptive Ambience System plugin.

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
	return EAssetTypeCategories::Sounds;
}

const TArray<FText>& FAmbiverseParameterAssetTypeActions::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		LOCTEXT("AssetAmbiverseSubMenu", "Ambiverse")
	};

	return SubMenus;
}
