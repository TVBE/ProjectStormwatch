// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

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

	/** The lifetime of the heat point. This describes how long the heat point has been active. */
	UPROPERTY(BlueprintGetter = GetLifetime)
	int Lifetime {0};

	/** The expiration time of the heat point. This describes how long the heat point will remain active before expiring. */
	UPROPERTY(BlueprintGetter = GetExpirationTime)
	int ExpirationTime {30};

#if WITH_EDITORONLY_DATA
	bool IsDebugVisEnabled {true};
	
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

public:
	void InitializeHeatPoint(const int RadiusValue, const int LifeTimeValue, const float HeatValue);

	void UpdateLifeTime(const int DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	void SetRadius(const int NewRadius);
	
	UFUNCTION(BlueprintCallable)
	void SetHeat(const float NewHeat);
	
	UFUNCTION(BlueprintCallable)
	void AddHeat(const float AddedHeat);
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE int GetRadius() const { return Radius; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE float GetHeat() const { return Heat; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE int GetLifeTime() const { return Lifetime; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE int GetExpirationTime() const { return ExpirationTime; }

#if WITH_EDITORONLY_DATA
	void SetDebugVisEnabled(bool IsEnabled);
#endif
	
};
