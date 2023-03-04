// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAudioComponent.generated.h"

class UMetaSoundSource;
class APlayerCharacter;
class UAudioComponent;

/** UPlayerAudioComponent is an Actor Component responsible for managing all audio specific to the player character. 
 *	This class provides a simple and convenient way for designers to customize the player's audio implementation.
 *	@Brief ActorComponent for managing player audio.
 */
UCLASS(Blueprintable, ClassGroup=(PlayerCharacter), Meta=(BlueprintSpawnableComponent) )
class UPlayerAudioComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** The AudioComponent for general player audio. */
	UPROPERTY(BlueprintGetter = GetBodyAudioComponent, VisibleAnywhere, Category = "PlayerAudioComponent", Meta = (DisplayName = "Body Audio Component"))
	UAudioComponent* BodyAudioComponent;

	/** The Metasound Asset for the body audio component. */
	UPROPERTY(EditAnywhere, Category = "PlayerAudioComponent", Meta = (DisplayName = "Body Audio Component Metasound Source"))
	TSoftObjectPtr<UMetaSoundSource> BodyAudioComponentSoundAsset; 
	
public:	
	/** Sets default values for this component's properties. */
	UPlayerAudioComponent();

	/** Called every frame. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void CleanupComponent();

public:
	/** Returns the body AudioComponent. */
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Body Audio Component"))
	FORCEINLINE UAudioComponent* GetBodyAudioComponent() const {return BodyAudioComponent; }
	
};
