#include "UPlayerSkeletalMeshComponent.h"

// Sets default values
AUPlayerSkeletalMeshComponent::AUPlayerSkeletalMeshComponent()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AUPlayerSkeletalMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AUPlayerSkeletalMeshComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
