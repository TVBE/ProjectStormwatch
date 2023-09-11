#include "PlayerCharacterConfigDataAsset.h"

// Sets default values
APlayerCharacterConfigDataAsset::APlayerCharacterConfigDataAsset()
{
	// Set this empty to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerCharacterConfigDataAsset::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerCharacterConfigDataAsset::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacterConfigDataAsset::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
