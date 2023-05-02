#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "Sound/Sound.h"
#include "Sound/SoundEffectPreset.h" // Added header for USoundEffectPreset class
#include "RoomReverbSettings.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FROSTBITE_API URoomReverbSettings : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URoomReverbSettings();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// The reverb effect asset to use
	UPROPERTY(EditAnywhere, Category = "Reverb Settings")
	UReverbEffect* Reverb;

	// The reverb component to apply the effect
	UPROPERTY(EditAnywhere, Category = "Reverb Settings")
	USoundEffectPreset* ReverbComponent;

	// The duration of the fade in/out effects
	UPROPERTY(EditAnywhere, Category = "Reverb Settings")
	float FadeInTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Reverb Settings")
	float FadeOutTime = 1.0f;

	// Overlap events
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};