// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "Factories/Factory.h"
#include "Components/AudioComponent.h"




class FReacousticSoundAssetTypeActions : public FAssetTypeActions_Base
{
public:
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual const TArray<FText>& GetSubMenus() const override;
	EVisibility GetThumbnailOverlayVisibility() const;
	virtual TSharedPtr<SWidget> GetThumbnailOverlay(const FAssetData& AssetData) const override;
	FReply OnThumbnailOverlayClicked() const;


protected:
	/** Plays the specified sound wave */
	void PlaySound(USoundBase* Sound) const;

	/** Stops any currently playing sounds */
	void StopSound() const;

	/** Return true if the specified sound is playing */
	bool IsSoundPlaying(USoundBase* Sound) const;

	/** Return true if the specified asset's sound is playing */
	bool IsSoundPlaying(const FAssetData& AssetData) const;

private:
	/** Handler for when PlaySound is selected */
	void ExecutePlaySound(TArray<TWeakObjectPtr<USoundBase>> Objects) const;

	/** Handler for when StopSound is selected */
	void ExecuteStopSound(TArray<TWeakObjectPtr<USoundBase>> Objects) const;
};

class FReacousticContentBrowserMenuExtension
{
public:
	static void RegisterMenus();
	static void ExecuteCreateReacousticSound(const struct FToolMenuContext& MenuContext);
};


