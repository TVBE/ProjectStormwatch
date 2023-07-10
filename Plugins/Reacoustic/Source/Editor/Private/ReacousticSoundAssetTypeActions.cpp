// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#include "ReacousticSoundAssetTypeActions.h"
#include "ReacousticSoundAsset.h"
#include "ReacousticEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FReacousticSoundAssetTypeActions::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ReacousticSound", "Reacoustic Sound");
}

UClass* FReacousticSoundAssetTypeActions::GetSupportedClass() const
{
	return UReacousticSoundAsset::StaticClass();
}

FColor FReacousticSoundAssetTypeActions::GetTypeColor() const
{
	return REACOUSTIC_ASSET_COLOR;
}

uint32 FReacousticSoundAssetTypeActions::GetCategories()
{
	return EAssetTypeCategories::Sounds;
}

const TArray<FText>& FReacousticSoundAssetTypeActions::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		LOCTEXT("AssetReacousticSubMenu", "Reacoustic")
	};

	return SubMenus;
}