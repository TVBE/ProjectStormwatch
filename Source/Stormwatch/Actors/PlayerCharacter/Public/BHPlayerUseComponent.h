// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "BHPlayerUseComponent.generated.h"

UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "BHPlayerCharacter")
class UBHPlayerUseComponent : public UActorComponent, public FBHPlayerCharacterComponent
{
	GENERATED_BODY()

private:
	/** The actor that is currently in use. */
	UPROPERTY()
	AActor* ActorInUse;

	/** The object that implements IUsableObject that is currently in use. This might be an actor, or an actor component. */
	UPROPERTY()
	UObject* ObjectInUse;

public:	
	UBHPlayerUseComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Begins using an object. The object is expected to implement the IUsableObject interface.
	 *	@Param UsableObject The object that implements the IUsableObject interface.
	 *	@Return Whether the object was successfully used.
	 */
	UFUNCTION()
	bool BeginUse(UObject* UsableObject);

	/** Stops using an object. The object is expected to implement the IUsableObject interface.
	 *	@Return Whether the object was successfully released from use.
	 */
	UFUNCTION()
	bool EndUse();

protected:
	virtual void BeginPlay() override;

private:
	/** Converts a UObject pointer to an AActor pointer.
	 *	Either by checking if the UObject is an AActor,
	 *	or by casting the UObject to an UActorComponent and retrieving the owner from the component. */
	UFUNCTION()
	AActor* GetActorFromObject(UObject* Object) const;

public:
	UFUNCTION(BlueprintPure, Category = "BHPlayerUseComponent")
	bool IsUsingObject() const;
	
	/** Returns the actor that is currently in use. */
	AActor* GetActorInUse() const { return ActorInUse; }

	/** Returns the object that is currently in use. This object implements the IUsableObject interface. */
	UObject* GetObjectInUse() const { return ObjectInUse; }
};
