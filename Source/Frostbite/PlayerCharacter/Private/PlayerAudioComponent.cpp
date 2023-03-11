// Copyright 2023 Barrelhouse

#include "PlayerAudioComponent.h"
#include "PlayerCharacter.h"

#include "Components/AudioComponent.h"
#include "MetasoundSource.h"

// Sets default values for this component's properties
UPlayerAudioComponent::UPlayerAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UPlayerAudioComponent::OnRegister()
{
	Super::OnRegister();
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if(!PlayerCharacter) {return; }

	/** Construct Audio Component. */
	BodyAudioComponent = Cast<UAudioComponent>(GetOwner()->AddComponentByClass(UAudioComponent::StaticClass(), false, FTransform(), false));
	BodyAudioComponent->bAutoActivate = false;
	BodyAudioComponent->bEditableWhenInherited = false;
	BodyAudioComponent->SetSound(BodyAudioComponentSoundAsset.LoadSynchronous());
}

/** Called when the game starts. */
void UPlayerAudioComponent::BeginPlay()
{
	Super::BeginPlay();
}

/** Called every frame. */
void UPlayerAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlayerAudioComponent::CleanupComponent()
{
	if(BodyAudioComponent)
	{
		if(BodyAudioComponent->IsPlaying())
		{
			BodyAudioComponent->Stop();
		}
		BodyAudioComponent->Deactivate();
		BodyAudioComponent->DestroyComponent();
		BodyAudioComponent = nullptr;
	}
}

void UPlayerAudioComponent::OnUnregister()
{
	CleanupComponent();
	Super::OnUnregister();
}

void UPlayerAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CleanupComponent();
	Super::EndPlay(EndPlayReason);
}





