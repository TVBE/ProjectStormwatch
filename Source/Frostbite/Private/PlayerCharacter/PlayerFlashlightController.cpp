// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerFlashlightController.h"

#include "PlayerCharacter.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values for this component's properties
UPlayerFlashlightController::UPlayerFlashlightController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	// ...
}


// Called when the game starts
void UPlayerFlashlightController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	
}


// Called every frame
void UPlayerFlashlightController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerFlashlightController::SetFlashlightEnabled(const bool Value)
{
	if(PlayerCharacter && PlayerCharacter->GetFlashlight() && PlayerCharacter->GetFlashlightSpringArm())
	{
		SetComponentTickEnabled(Value);
		PlayerCharacter->GetFlashlightSpringArm()->SetComponentTickEnabled(Value);
		PlayerCharacter->GetFlashlight()->SetVisibility(Value);
	}
}

bool UPlayerFlashlightController::IsFlashlightEnabled()
{
	if(PlayerCharacter && PlayerCharacter->GetFlashlight())
	{
		return PlayerCharacter->GetFlashlight()->IsVisible();
	}
	return false;
}

