#include "UPlayerCameraComponent.h"

// Sets default values
AUPlayerCameraComponent::AUPlayerCameraComponent()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AUPlayerCameraComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AUPlayerCameraComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
