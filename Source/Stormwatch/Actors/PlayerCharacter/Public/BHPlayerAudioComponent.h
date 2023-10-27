// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "BHPlayerAudioComponent.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHPlayerAudioComponent : public UBHPlayerCharacterComponent
{
	GENERATED_BODY()

public:
	UBHPlayerAudioComponent();

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
