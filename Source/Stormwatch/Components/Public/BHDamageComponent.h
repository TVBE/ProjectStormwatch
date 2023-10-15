// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "BHDamageComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamagePercentageChangedDelegate, const float, NewPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamageThresholdReachedDelegate);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Physics", Meta = (BlueprintSpawnableComponent))
class UBHDamageComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UBHDamageComponent();
	
	UFUNCTION(BlueprintCallable)
	void ResetDamage();
	
	UPROPERTY(BlueprintAssignable)
	FOnDamagePercentageChangedDelegate OnDamaged;
	
	UPROPERTY(BlueprintAssignable)
	FOnDamageThresholdReachedDelegate OnDamageThresholdReached;

private:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void OnStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void AddDamage(const float Value);
	
	void SetDamageLevel(const float Value);

	void UpdateDamagePercentage();

	void HandleDamageThresholdReached();

	void StartCooldown();
	
	UFUNCTION()
	void HandleCooldownFinished();

	// The damage threshold for the component to broadcast its OnDamageThresholdReached delegate.
	UPROPERTY(EditAnywhere, Meta = (Units = "Newtons"))
	float Threshold = 100000.0f;

	// The minimum impact force required to increment the damage.
	UPROPERTY(EditAnywhere, Meta = (Units = "Newtons"))
	float ImpulseForceThreshold = 500.0f;
	
	UPROPERTY(EditAnywhere, Category = "VFX", Meta = (InlineEditConditionToggle))
	bool PlayParticleEffects = false;
	
	UPROPERTY(EditAnywhere, Category = "VFX", Meta = (DisplayName = "Destruction Particle Effect",
		EditCondition = "PlayParticleEffects"))
	class UNiagaraSystem* DestructionNiagaraSystem = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio", Meta = (InlineEditConditionToggle))
	bool PlayAudioEffects = false;
	
	UPROPERTY(EditAnywhere, Category = "Audio", Meta = (DisplayName = "Destruction Sound Effect",
		EditCondition = "PlayAudioEffects"))
	USoundBase* DestructionSound = nullptr;
	
	float DamageLevel = 0.0f;
	float DamagePercentage = 0.0f;

	bool IsThresholdReached = false;

	bool IsCooldownActive = false;
	
	UPROPERTY()
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY()
	FTimerHandle CooldownTimerHandle;
};
