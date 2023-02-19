// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAudioController.h"
#include "PlayerCharacter.h"

// Sets default values for this component's properties
UPlayerAudioController::UPlayerAudioController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Initializes the component. Occurs at level startup or actor spawn. This is before BeginPlay.
void UPlayerAudioController::InitializeComponent()
{
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if(!PlayerCharacter)
	{
		return;
	}
	Super::InitializeComponent();
	
}

// Called when the game starts
void UPlayerAudioController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UPlayerAudioController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

