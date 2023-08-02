// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class UAmbiverseElementAsset;

class FAssetTypeActions_AmbiverseElement : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual const TArray<FText>& GetSubMenus() const override;
	virtual TSharedPtr<SWidget> GetThumbnailOverlay(const FAssetData& AssetData) const override;

private:
	/** Plays the sound of the element. */
	void PlayElement(TWeakObjectPtr<UAmbiverseElementAsset> Object) const;

	/** Stops any currently playing sounds. */
	void StopElement() const;
	
	/** Returns true if the specified asset's sound is playing */
	bool IsElementPlaying(const FAssetData& AssetData) const;
};
