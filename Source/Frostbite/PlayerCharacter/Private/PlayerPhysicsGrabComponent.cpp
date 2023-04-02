// Copyright (c) 2022-present Barrelhouse
// Written by Nino Saglia
// This source code is part of the project Frostbite

#include "PlayerPhysicsGrabComponent.h"

#include "PlayerCharacter.h"

void UPlayerPhysicsGrabComponent::OnRegister()
{
	Super::OnRegister();

	/** Load the configuration asset. If no configuration asset is provided, construct a default instance of the configuration asset instead. */
	if (!Configuration)
	{
		Configuration = ConfigurationAsset.LoadSynchronous();
		if (!Configuration)
		{
			Configuration = NewObject<UPlayerPhysicsGrabConfiguration>();
			Configuration->AddToRoot();

		}
	}
	if (const APlayerCharacter* PlayerCharacter {Cast<APlayerCharacter>(GetOwner())})
	{
		Camera = PlayerCharacter->GetCamera();
	}
}

void UPlayerPhysicsGrabComponent::GrabObject(AActor* ObjectToGrab)
{
	// check if there's a reference and cast to static mesh component to get a ref to the first static mesh.
	if (!ObjectToGrab){return;}
	UStaticMeshComponent* StaticMeshComponent = nullptr;
	StaticMeshComponent = Cast<UStaticMeshComponent>(ObjectToGrab->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (StaticMeshComponent)
	{
		GrabComponentAtLocation(StaticMeshComponent, NAME_None,StaticMeshComponent->GetCenterOfMass());
	}
}

void UPlayerPhysicsGrabComponent::ReleaseObject()
{
	ReleaseComponent();
}

void UPlayerPhysicsGrabComponent::UpdateDesiredLocation(float ZoomAxisValue)
{
	if (!GrabbedComponent) return;
	CurrentZoomLevel = FMath::Clamp(CurrentZoomLevel + ZoomAxisValue * Configuration->ZoomSpeed, Configuration->MinZoomLevel, Configuration->MaxZoomLevel);

	// Calculate the desired location based on the forward vector and zoom level
	
}

void UPlayerPhysicsGrabConfiguration::ApplyToPlayerPhysicsGrabComponent(const UPlayerPhysicsGrabComponent* Component)
{
	
}

