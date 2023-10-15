#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UPlayerSkeletalMeshComponent.generated.h"

UCLASS()
class STORMWATCH_API AUPlayerSkeletalMeshComponent : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AUPlayerSkeletalMeshComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
