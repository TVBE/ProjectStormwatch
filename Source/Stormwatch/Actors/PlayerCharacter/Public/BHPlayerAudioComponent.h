// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "BHPlayerAudioComponent.generated.h"

// Manages all player character audio. Owns the PlayerCharacters main audio components.
// Any audio specific logic for the player character should be implemented in a class derived from this.
UCLASS(Blueprintable, BlueprintType, ClassGroup = "BHPlayerCharacter")
class STORMWATCH_API UBHPlayerAudioComponent : public UActorComponent, public FBHPlayerCharacterComponent
{
	GENERATED_BODY()

public:
	UBHPlayerAudioComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintGetter)
	class UAudioComponent* GetBodyAudioComponent() const { return MainAudioComponent; }

protected:
	virtual void PostInitProperties() override;
	virtual void OnUnregister() override;

private:
	UPROPERTY(BlueprintGetter = GetBodyAudioComponent)
	UAudioComponent* MainAudioComponent = nullptr;
	
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Body Audio Component Metasound Source"))
	class UMetaSoundSource* BodyAudioComponentSoundAsset = nullptr;
};
