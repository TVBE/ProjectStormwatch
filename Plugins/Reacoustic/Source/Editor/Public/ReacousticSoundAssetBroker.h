// Copyright Nino

#pragma once
#include "ReacousticSoundAsset.h"
#include "ComponentAssetBroker.h"
#include "ReacousticComponent.h"
#include "Engine/Engine.h"

/* FReacousticSoundAssetBroker
 *
 * Enables dragging a reacoustic sound asset onto an actor in the outliner.
 *
 * TODO: figure out if i want to add functionality to add assets to meshes in the viewport direcrly.
 */

class FReacousticSoundAssetBroker : public IComponentAssetBroker
{
public:
	UClass* GetSupportedAssetClass() override
	{
		return UReacousticSoundAsset::StaticClass();
	}

	virtual bool AssignAssetToComponent(UActorComponent* InComponent, UObject* InAsset) override
	{
		if (UReacousticComponent* Component = Cast<UReacousticComponent>(InComponent))
		{
			UReacousticSoundAsset* SoundAsset = Cast<UReacousticSoundAsset>(InAsset);

			if ((SoundAsset != nullptr) || (InAsset == nullptr))
			{
				Component->ReacousticSoundAsset = SoundAsset;
				return true;
			}
		}

		return false;
	}

	virtual UObject* GetAssetFromComponent(UActorComponent* InComponent) override
	{
		if (UReacousticComponent* Component = Cast<UReacousticComponent>(InComponent))
		{
			if ((Component->ReacousticSoundAsset != nullptr) && (Component->ReacousticSoundAsset->IsAsset()))
			{
				return Component->ReacousticSoundAsset;
			}
		}
		return nullptr;
	}
};
