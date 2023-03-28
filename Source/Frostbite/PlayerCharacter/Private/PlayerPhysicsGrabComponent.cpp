// Copyright 2023 Barrelhouse


#include "PlayerPhysicsGrabComponent.h"


UPlayerPhysicsGrabComponent::UPlayerPhysicsGrabComponent(){};
void UPlayerPhysicsGrabComponent::GrabObject(AActor* ObjectToGrab)
{
	if (!ObjectToGrab){return;}
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	StaticMeshComponent = Cast<UStaticMeshComponent>(ObjectToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (StaticMeshComponent)
	{
		// the component was found, do something with it
	}
}

void UPlayerPhysicsGrabComponent::ReleaseObject()
{
	ReleaseComponent();
}

void UPlayerPhysicsGrabComponent::UpdateZoomLevel()
{
	
}

void UPlayerPhysicsGrabComponent::UpdateDesiredLocation(float ZoomAxisValue)
{
	if (!GrabbedComponent) return;
	CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel + ZoomAxisValue * ZoomSpeed, MinZoomLevel, MaxZoomLevel);

	// Calculate the desired location based on the forward vector and zoom level
	
}

