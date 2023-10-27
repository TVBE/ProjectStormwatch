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
	
	BodyAudioComponent = Cast<UAudioComponent>(GetOwner()->AddComponentByClass(UAudioComponent::StaticClass(), false, FTransform(), false));
	BodyAudioComponent->bAutoActivate = false;
	BodyAudioComponent->bEditableWhenInherited = false;
	BodyAudioComponent->SetSound(BodyAudioComponentSoundAsset);
}

void UBHPlayerAudioComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBHPlayerAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBHPlayerAudioComponent::OnUnregister()
{
	if (BodyAudioComponent)
	{
		if (BodyAudioComponent->IsPlaying())
		{
			BodyAudioComponent->Stop();
		}
		BodyAudioComponent->Deactivate();
		BodyAudioComponent->DestroyComponent();
	}
	
	Super::OnUnregister();
}






