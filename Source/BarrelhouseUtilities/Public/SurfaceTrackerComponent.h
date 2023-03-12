// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurfaceTrackerComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Utility), Meta = (BlueprintSpawnableComponent) )
class USurfaceTrackerComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	/** The desired height that the component should try to stay above the surface. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SurfaceTrackerComponent", Meta = (DisplayName = "Tracking Height"))
	float TrackingHeight {150.0f};

	/** The maximum allowed deviation from the owner of this component on the Z axis. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SurfaceTrackerComponent", Meta = (DisplayName = "Deviation Range"))
	float DeviationRange {750.0f};

	/** The interpolation speed for this component to try and reach its target. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SurfaceTrackerComponent", Meta = (DisplayName = "Interpolation Speed"))
	float InterpolationSpeed {2.0f};

	/** The collision query range to be used for detecting geometry underneath the component. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SurfaceTrackerComponent", Meta = (DisplayName = "Tracking Length"))
	float TrackingLength {1000};

#if WITH_EDITORONLY_DATA
	/** When enabled, the component will draw a debug sphere at its location. */
	UPROPERTY(BlueprintReadWrite, Category = "SurfaceTrackerComponent|Debugging", Meta = (DisplayName = "Enable Debugging Visualisation"))
	bool EnableDebugVisualisation {false};
#endif
	
public:	
	// Sets default values for this component's properties
	USurfaceTrackerComponent();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITOR
	// Draws a debug sphere at the component's location
	void DrawDebugVisualisation();
#endif
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	/** Returns the distance between the component and any geometry underneath it. */
	static float GetDistanceToSurface(const UWorld* World, const FVector Location, const float TraceLength);


		
};
