// Copyright 2023 Barrelhouse

#include "PlayerVfxController.h"
#include "PlayerCharacter.h"
#include "Frostbite/Frostbite.h"

// Sets default values for this component's properties
UPlayerVfxController::UPlayerVfxController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Initializes the component. Occurs at level startup or actor spawn. This is before BeginPlay.
void UPlayerVfxController::InitializeComponent()
{
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if(!PlayerCharacter)
	{
		return;
	}
	
	Super::InitializeComponent();
}

// Called when the game starts
void UPlayerVfxController::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}




// Called every frame
void UPlayerVfxController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

