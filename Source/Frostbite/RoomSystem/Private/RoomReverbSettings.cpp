
#include "RoomReverbSettings.h"

#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"

URoomReverbSettings::URoomReverbSettings()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URoomReverbSettings::BeginPlay()
{
	Super::BeginPlay();

	// Get the parent bounding box component
	UBoxComponent* ParentBox = GetOwner()->FindComponentByClass<UBoxComponent>();
	if (ParentBox)
	{
		// Bind the overlap events
		ParentBox->OnComponentBeginOverlap.AddDynamic(this, &URoomReverbSettings::OnOverlapBegin);
		ParentBox->OnComponentEndOverlap.AddDynamic(this, &URoomReverbSettings::OnOverlapEnd);
	}
}

void URoomReverbSettings::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Unbind the overlap events
	UBoxComponent* ParentBox = GetOwner()->FindComponentByClass<UBoxComponent>();
	if (ParentBox)
	{
		ParentBox->OnComponentBeginOverlap.RemoveDynamic(this, &URoomReverbSettings::OnOverlapBegin);
		ParentBox->OnComponentEndOverlap.RemoveDynamic(this, &URoomReverbSettings::OnOverlapEnd);
	}
	//CurrentFadeTime = 0.0f;
	// CurrentVolume = 0.0f;
	//ReverbSettingsAsset->Settings.RoomDampening = 0.0f; // Set initial value for RoomDampening
	// ReverbSettingsAsset->Settings.ReflectionsDelay = 0.0f; // Set initial value for ReflectionsDelay
}




void URoomReverbSettings::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		if (ReverbComponent)
		{
			// Fade in the reverb effect
		//	ReverbComponent->FadeInEffect(ReverbEffect, FadeInTime, 1.0f);
		}
	}
}

void URoomReverbSettings::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<APlayerCharacter>())
	{
		if (ReverbComponent)
		{
			// Fade out the reverb effect
			//ReverbComponent->FadeOutEffect(FadeOutTime, 0.0f);
		}
	}
}


