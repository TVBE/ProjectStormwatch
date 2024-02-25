// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BHPhysicsMeshActor.generated.h"

// Base class for physics meshes. Does not implement any type of interactivity.
// Users are responsible for implementing any interaction logic when inheriting from this class.
UCLASS(Abstract, Blueprintable, BlueprintType, Hidecategories = ("Input"), ClassGroup = "Barrelhouse")
class STORMWATCH_API ABHPhysicsMeshActor : public AActor
{
	GENERATED_BODY()

public:
	ABHPhysicsMeshActor();
	
	UFUNCTION(BlueprintGetter)
	UStaticMeshComponent* GetStaticMeshComponent() const;

	UFUNCTION(BlueprintGetter)
	class UReacousticComponent* GetReacousticComponent() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Physics")
	bool bAutoDeterminePhysicsAttributes = false;

private:
	UPROPERTY(BlueprintGetter = GetStaticMeshComponent, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintGetter= GetReacousticComponent, Category = "Components")
	UReacousticComponent* ReacousticComponent;
};
