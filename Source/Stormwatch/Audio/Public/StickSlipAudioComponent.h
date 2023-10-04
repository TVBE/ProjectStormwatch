#pragma once
#include "ReacousticSoundAsset.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "StickSlipAudioComponent.generated.h"



/**
 * The stick-slip sound model algorithm is designed to mimic the phenomenon where an object alternates between sticking to a surface and slipping over it.
 * This is common in various real-world interactions like squeaky doors, dragging objects, or even musical instruments like the violin.
 * this algorithm triggers Reacoustic sound transients according to this behavior.
 */

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Audio", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UStickSlipAudioComponent : public UActorComponent
{
	GENERATED_BODY()

	
public:
	
protected:
	
	/**We get the transient data and wave file from the Reacoustic sound asset.*/
	UPROPERTY(EditInstanceOnly, BlueprintGetter = GetReacousticSoundAsset)
	UReacousticSoundAsset* ReacousticSoundAsset;

	/**How much the velocity of the actor will drive the stick slip model.*/
	UPROPERTY(EditInstanceOnly, BlueprintGetter = GetDirectionalSensitivity)
	float DirectionalSensitivity{1.0};

	/**How much the angular velocity of the actor will drive the stick slip model.*/
	UPROPERTY(EditInstanceOnly, BlueprintGetter = GetRotationalSensitivity)
	float RotationalSensitivity{1.0};

	/**The minimum interval of the distribution of trigger points for the sound.*/
	UPROPERTY(EditInstanceOnly, BlueprintGetter = GetJitterMinInterval)
	float JitterMinInterval{0.1};
	
	/**The maximum interval of the distribution of trigger points for the sound.*/
	UPROPERTY(EditInstanceOnly, BlueprintGetter =GetJitterMaxInterval )
	float JitterMaxInterval{1.0};

public:
	/** Getters for Blueprint and code */
	UFUNCTION(BlueprintPure, Category = "StickSlip")
	UReacousticSoundAsset* GetReacousticSoundAsset() const {return ReacousticSoundAsset;}

	UFUNCTION(BlueprintPure, Category = "StickSlip")
	float GetDirectionalSensitivity() const {return DirectionalSensitivity;}

	UFUNCTION(BlueprintPure, Category = "StickSlip")
	float GetRotationalSensitivity() const {return RotationalSensitivity;}

	UFUNCTION(BlueprintPure, Category = "StickSlip")
	float GetJitterMinInterval() const {return JitterMinInterval;}

	UFUNCTION(BlueprintPure, Category = "StickSlip")
	float GetJitterMaxInterval() const {return JitterMaxInterval;}

	/** This isn't exposed to bp by default so i expose angular V here.*/
	//UFUNCTION(BlueprintPure, Category = "StickSlip")
	//float GetAnglularVelocity(AActor* A)
	//const{return A->GetRootComponent()->GetPhysicsAngula;}
	
private:
	
	UStickSlipAudioComponent();
	void BeginPlay();
	void ScheduleNextEvent();
	void TriggerEvent();
};
