// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "RoomComponent.h"

#include "RoomVolume.h"
#include "Engine/World.h"

URoomComponent::URoomComponent()
{
}

void URoomComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ARoomVolume* RoomVolume = Cast<ARoomVolume>(GetOwner());)
	{
		RoomVolume->OnPawnEnter.AddDynamic(this, &URoomComponent::HandlePawnEnter);
		RoomVolume->OnPawnLeave.AddDynamic(this, &URoomComponent::HandlePawnLeave);
	}
}

void URoomComponent::HandlePawnEnter(APawn* Pawn)
{
	EventPawnEnter(Pawn);
}

void URoomComponent::HandlePawnLeave(APawn* Pawn)
{
	EventPawnLeave(Pawn);
}

void URoomComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (ARoomVolume* RoomVolume = Cast<ARoomVolume>(GetOwner());)
	{
		RoomVolume->OnPawnEnter.RemoveDynamic(this, &URoomComponent::HandlePawnEnter);
		RoomVolume->OnPawnLeave.RemoveDynamic(this, &URoomComponent::HandlePawnLeave);
	}
}

void URoomComponent::EventPawnEnter_Implementation(APawn* Pawn)
{
}

void URoomComponent::EventPawnLeave_Implementation(APawn* Pawn)
{
}


