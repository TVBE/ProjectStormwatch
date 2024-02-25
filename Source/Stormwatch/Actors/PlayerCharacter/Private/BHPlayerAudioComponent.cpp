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

	MainAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MainAudioComponent"));
	MainAudioComponent->bAutoActivate = false;
	MainAudioComponent->bEditableWhenInherited = true;
}

void UBHPlayerAudioComponent::PostInitProperties()
{
	Super::PostInitProperties();

	if (ensure(MainAudioComponent))
	{
		MainAudioComponent->SetSound(BodyAudioComponentSoundAsset);
	}
}

void UBHPlayerAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBHPlayerAudioComponent::OnUnregister()
{
	Super::OnUnregister();

	if (ensure(MainAudioComponent))
	{
		if (MainAudioComponent->IsPlaying())
		{
			MainAudioComponent->Stop();
		}
	}
}