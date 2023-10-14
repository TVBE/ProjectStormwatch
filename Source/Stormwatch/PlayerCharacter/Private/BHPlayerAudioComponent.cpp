// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerAudioComponent.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerController.h"
#include "Components/AudioComponent.h"
#include "MetasoundSource.h"

UBHPlayerAudioComponent::UBHPlayerAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBHPlayerAudioComponent::OnRegister()
{
	Super::OnRegister();
	
	ABHPlayerCharacter* PlayerCharacter = Cast<ABHPlayerCharacter>(GetOwner());
	if (!PlayerCharacter) {return; }

	BodyAudioComponent = Cast<UAudioComponent>(GetOwner()->AddComponentByClass(UAudioComponent::StaticClass(), false, FTransform(), false));
	BodyAudioComponent->bAutoActivate = false;
	BodyAudioComponent->bEditableWhenInherited = false;
	BodyAudioComponent->SetSound(BodyAudioComponentSoundAsset.LoadSynchronous());
}

void UBHPlayerAudioComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBHPlayerAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBHPlayerAudioComponent::CleanupComponent()
{
	if (BodyAudioComponent)
	{
		if (BodyAudioComponent->IsPlaying())
		{
			BodyAudioComponent->Stop();
		}
		BodyAudioComponent->Deactivate();
		BodyAudioComponent->DestroyComponent();
		BodyAudioComponent = nullptr;
	}
}

void UBHPlayerAudioComponent::OnUnregister()
{
	CleanupComponent();
	Super::OnUnregister();
}

void UBHPlayerAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CleanupComponent();
	Super::EndPlay(EndPlayReason);
}





