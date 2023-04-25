// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "PlayerBodyCollisionComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter", Meta = (DisplayName = "Body Collision Component",
		ShortToolTip = "Collision component that pushes physics actors away from the player's body."))
class FROSTBITE_API UPlayerBodyCollisionComponent : public UCapsuleComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<UPrimitiveComponent*> PushedComponents;

	UPROPERTY(EditDefaultsOnly, Category = "Body Collision", Meta = (DisplayName = "Push Force"))
	float PushForce {500.0f};

	UPROPERTY(EditDefaultsOnly, Category = "Body Collision", Meta = (DisplayName = "Bounce Force"))
	float BounceForce {1000.0f};

	UPROPERTY(EditDefaultsOnly, Category = "Body Collision", Meta = (DisplayName = "Bounce Threshold", Units = "Kilograms"))
	float BounceThreshold {2.0f};

	/** If true, we're currently pushing objects. */
	UPROPERTY(BlueprintGetter = GetIsPushingObjects, Category = "Body Collision", Meta = (DisplayName = "Is Pushing Objects"))
	bool IsPushingObjects {false};

public:
	UPlayerBodyCollisionComponent();

	UFUNCTION(BlueprintCallable, Category = "Body Collision")
	float GetTotalMassOfPushedObjects() const;

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	/** Returns whether the component is currently pushing objects. */
	UFUNCTION(BlueprintGetter, Category = "Body Collision", Meta = (DisplayName = "Is Pushing Objects"))
	FORCEINLINE bool GetIsPushingObjects() const { return IsPushingObjects; }
};
