// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeatPoint.generated.h"

class USphereComponent;

UCLASS(ClassGroup = "Nightstalker")
class AHeatPoint : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	USphereComponent* SphereComponent;
	
	UPROPERTY(BlueprintGetter = GetRadius)
	int Radius;

	UPROPERTY(BlueprintGetter = GetHeat)
	float Heat;

	UPROPERTY(BlueprintGetter = GetLifetime)
	int Lifetime;
	
	UPROPERTY()
	FTimerHandle LifetimeTimerHandle;

#if WITH_EDITORONLY_DATA
	bool IsDebugVisEnabled {false};
	
	UPROPERTY(Transient)
	UStaticMeshComponent* DebugSphereMesh;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* DebugMaterial;
#endif

public:	
	AHeatPoint();

private:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void Update();

	UFUNCTION()
	void HandleLifeTimeUpdate();

public:
	void InitializeHeatPoint(const int RadiusValue, const int LifeTimeValue, const float HeatValue);
	
	UFUNCTION(BlueprintCallable)
	void SetRadius(const int NewRadius);
	
	UFUNCTION(BlueprintCallable)
	void SetHeat(const float NewHeat);

	UFUNCTION(BlueprintCallable)
	void AddLifeTime(const int Time);

	UFUNCTION(BlueprintCallable)
	void AddHeat(const float AddedHeat);

	UFUNCTION(BlueprintGetter)
	FORCEINLINE int GetRadius() const { return Radius; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE float GetHeat() const { return Heat; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE int GetLifeTime() const { return Lifetime; }

#if WITH_EDITORONLY_DATA
	void SetDebugVisEnabled(bool IsEnabled);
#endif
	
};
