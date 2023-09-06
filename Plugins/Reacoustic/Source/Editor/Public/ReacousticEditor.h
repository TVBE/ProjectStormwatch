// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReacousticSoundAssetTypeActions.h"


class FReacousticEditorModule: public IModuleInterface, public TSharedFromThis<FReacousticEditorModule>
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	UFUNCTION()
	void OnDragStarted(const TArray<FAssetData>& AssetDatas, UActorFactory* ActorFactory);



private:
	TSharedPtr<FReacousticSoundAssetTypeActions> ReacousticSoundAssetTypeActions;


#define REACOUSTIC_ASSET_COLOR FColor(0, 255, 130)
	
};