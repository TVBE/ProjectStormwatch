// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExteriorWindAudioComponent.generated.h"

class UMetaSoundSource;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Audio), Meta = (BlueprintSpawnableComponent) )
class UExteriorWindAudioComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** The direction of the wind component, This will dictate in which direction the collision queries will be performed. Treat this value as the world rotation yaw value of this component. */
	UPROPERTY(BlueprintReadWrite, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "Direction", AllowPrivateAccess = "true", BlueprintProtected))
	float Direction {0.0f};

private:
	/** The AudioComponent that is added to the owner of this actor to play wind audio on. */
	UPROPERTY(BlueprintGetter = GetAudioComponent, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "Audio Component"))
	UAudioComponent* AudioComponent;

	/** The MetaSound source that is played through the audio component. */
	UPROPERTY(BlueprintGetter = GetMetaSoundSource, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "MetaSound Source"))
	UMetaSoundSource* MetaSoundSource;
	
public:	
	// Sets default values for this component's properties
	UExteriorWindAudioComponent();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when before the object is destroyed or when the game ends.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Called when the component is initialized, but before BeginPlay
	virtual void InitializeComponent() override;

public:
	/** Returns the audio component that is used for playing wind audio. */
	UFUNCTION(BlueprintGetter, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "Audio Component"))
	FORCEINLINE UAudioComponent* GetAudioComponent() const {return AudioComponent; }

	/** Returns the MetaSound Source for the audio component that is used for playing wind audio. */
	UFUNCTION(BlueprintGetter, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "MetaSound Source"))
	FORCEINLINE UMetaSoundSource* GetMetaSoundSource() const {return MetaSoundSource; }

	

		
};
