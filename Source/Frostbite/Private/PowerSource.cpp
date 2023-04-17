// Copyright Notice


#include "PowerSource.h"

// Sets default values
APowerSource::APowerSource()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APowerSource::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APowerSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

