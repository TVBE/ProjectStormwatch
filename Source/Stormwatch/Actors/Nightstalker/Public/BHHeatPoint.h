// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BHHeatPoint.generated.h"

class USphereComponent;

UCLASS(ClassGroup = "Nightstalker")
class STORMWATCH_API ABHHeatPoint : public AActor
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
	int Lifetime = 0;

	/** The expiration time of the heat point. This describes how long the heat point will remain active before expiring. */
	UPROPERTY(BlueprintGetter = GetExpirationTime)
	int ExpirationTime = 60;

#if WITH_EDITORONLY_DATA
	bool bDebugVisEnabled = false;
	
	UPROPERTY(Transient)
	UStaticMeshComponent* DebugSphereMesh;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* DebugMaterial;
#endif

public:	
	ABHHeatPoint();
	
	void InitializeHeatPoint(const int RadiusValue, const int ExpirationValue, const float HeatValue);

	void UpdateLifeTime(const int DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	void SetRadius(const int NewRadius);
	
	UFUNCTION(BlueprintCallable)
	void SetHeat(const float NewHeat);
	
	UFUNCTION(BlueprintCallable)
	void AddHeat(const float AddedHeat);

	UFUNCTION(BlueprintCallable)
	void DetractHeat(const float HeatToDeduct);

#if WITH_EDITORONLY_DATA
	void SetDebugVisEnabled(bool IsEnabled);
#endif

private:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintGetter)
	FORCEINLINE int GetRadius() const { return Radius; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE float GetHeat() const { return Heat; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE int GetLifeTime() const { return Lifetime; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE int GetExpirationTime() const { return ExpirationTime; }
};
