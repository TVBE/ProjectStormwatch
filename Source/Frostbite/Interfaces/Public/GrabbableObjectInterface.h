// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "GrabbableObjectInterface.generated.h"

/** Defines whether the actor requires one or two hands to perform the interaction. */
UENUM(BlueprintType)
enum class EGrabType : uint8
{
	OneHanded			UMETA(DisplayName = "One Handed"),
	TwoHanded			UMETA(DisplayName = "Two Handed"),
};

/** Interface for grabbable objects. This interface can be implemented in any UObject derived class. */
UINTERFACE(Blueprintable, Meta = (DisplayName = "Grabbable Object Interface", ShortToolTip = "Interface for grabbable objects."))
class FROSTBITE_API UGrabbableObject : public UInterface
{
	GENERATED_BODY()
};

class FROSTBITE_API IGrabbableObject
{
	GENERATED_BODY()

public:
	/** Starts grabbing the object.
	 *	@Param Instigator The actor that instigated the interaction.
	 *	@Return Whether the object implements a special action on grab.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Use"))
	bool BeginGrab(const AActor* Interactor);

	/** Stop grabbing the object.
	 *	@Param Instigator The actor that instigated the interaction.
	 *	@Return Whether the object implements a special action on release.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Disuse"))
	bool EndGrab(const AActor* Interactor);
	
	/** Returns the interaction hand type of the object. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Get Interaction Hand Type"))
	EGrabType GetGrabType() const;
};