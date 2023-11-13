// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "BHPlayerAudioComponent.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "BHPlayerCharacter", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHPlayerAudioComponent : public UBHPlayerCharacterComponent
{
	GENERATED_BODY()

public:
	UBHPlayerAudioComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintGetter)
	class UAudioComponent* GetBodyAudioComponent() const { return BodyAudioComponent; }

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintGetter = GetBodyAudioComponent)
	UAudioComponent* BodyAudioComponent;
	
	UPROPERTY(EditAnywhere, Meta = (DisplayName = "Body Audio Component Metasound Source"))
	class UMetaSoundSource* BodyAudioComponentSoundAsset;
};
