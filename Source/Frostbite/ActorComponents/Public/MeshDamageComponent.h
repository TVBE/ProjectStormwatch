// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeshDamageComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamagePercentageChangedDelegate, const float, NewPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamageThresholdReachedDelegate);

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Physics", Meta = (BlueprintSpawnableComponent, Displayname = "Damage Component"))
class UMeshDamageComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	/** Delegate that is called when the damage percentage of the component is changed. */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnDamagePercentageChangedDelegate OnDamaged;

	/** Delegate that is called when the damage threshold of the component is reached. */
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnDamageThresholdReachedDelegate OnDamageThresholdReached;

	/** The damage threshold for the component to broadcast it's OnDamageThresholdReached delegate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (Units = "Newtons"))
	float Threshold {100000.0f};

	/** The minimum impact force required to increment the damage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (DisplayName = "Minimum Required Force", Units = "Newtons"))
	float ImpulseForceThreshold {500.0f};

private:
	/** The current damage level. */
	float DamageLevel {0.0f};

	/** The current damage percentage. */
	float DamagePercentage {0.0f};

	bool IsThresholdReached {false};

	bool IsCooldownActive {false};
	
	UPROPERTY()
	UStaticMeshComponent* MeshComponent;

	FTimerHandle CooldownTimerHandle;
	
public:	
	UMeshDamageComponent();

	/** Resets the damage component. */
	UFUNCTION(BlueprintCallable)
	void ResetDamage();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnStaticMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void AddDamage(const float Value);
	
	void SetDamageLevel(const float Value);

	void UpdateDamagePercentage();

	void StartCooldown();
	
	UFUNCTION()
	void HandleCooldownFinished();
	
};
