// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "PlayerCharacterComponent.h"
#include "PlayerAudioComponent.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter",
	   Meta = (BlueprintSpawnableComponent, DisplayName = "Player Audio Component", ShortToolTip = "Audio component for the player character."))
class STORMWATCH_API UPlayerAudioComponent : public UPlayerCharacterComponent
{
	GENERATED_BODY()

public:
	UPlayerAudioComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Returns the body AudioComponent. */
	UFUNCTION(BlueprintGetter)
	class UAudioComponent* GetBodyAudioComponent() const { return BodyAudioComponent; }

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The AudioComponent for general player audio. */
	UPROPERTY(BlueprintGetter = GetBodyAudioComponent)
	UAudioComponent* BodyAudioComponent;

	/** The Metasound Asset for the body audio component. */
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Body Audio Component Metasound Source"))
	TSoftObjectPtr<class UMetaSoundSource> BodyAudioComponentSoundAsset;

	void CleanupComponent();
};
