// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PlayerFootCollisionComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter",
	   Meta = (DisplayName = "Foot Collision Component", 
			   ShortToolTip = "Collision component that pushes physics actors away from the player's feet."))
class STORMWATCH_API UPlayerFootCollisionComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPlayerFootCollisionComponent();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
						bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* CollisionSphere;

	UPROPERTY(EditDefaultsOnly)
	float CollisionSphereRadius {30.0f};

	UPROPERTY(EditDefaultsOnly, Meta = (DisplayName = "Base Push Strength"))
	float MaxPushStrength {20.0f};

	/** Determines how much objects directly in front of the sphere are pushed to the side. */
	UPROPERTY(EditDefaultsOnly)
	float LateralOffsetFactor {0.5f};

	/** If true, we scale the push strength to the weight of the object. */
	UPROPERTY(EditDefaultsOnly)
	bool bScaleForceByMass {true};	
};
