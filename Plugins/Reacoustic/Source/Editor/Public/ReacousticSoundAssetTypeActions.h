// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Factories/Factory.h"




class FReacousticSoundAssetTypeActions : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual const TArray<FText>& GetSubMenus() const override;
};

class FReacousticContentBrowserMenuExtension
{
public:
	static void RegisterMenus();
	static void ExecuteCreateReacousticSound(const struct FToolMenuContext& MenuContext);
};


