// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHGrabbableObjectInterface.generated.h"

UENUM(BlueprintType)
enum class EBHGrabType : uint8
{
	OneHanded			UMETA(DisplayName = "One Handed"),
	TwoHanded			UMETA(DisplayName = "Two Handed"),
};

UINTERFACE(Blueprintable)
class STORMWATCH_API UBHGrabbableObject : public UInterface
{
	GENERATED_BODY()
};

class STORMWATCH_API IBHGrabbableObject
{
	GENERATED_BODY()

public:
	/** Starts grabbing the object.
	 *	@Param Instigator The actor that instigated the interaction.
	 *	@Return Whether the object implements a special action on grab.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable Object")
	bool BeginGrab(const AActor* Interactor);

	/** Stop grabbing the object.
	 *	@Param Instigator The actor that instigated the interaction.
	 *	@Return Whether the object implements a special action on release.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable Object")
	bool EndGrab(const AActor* Interactor);
	
	/** Returns the interaction hand type of the object. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbable Object")
	EBHGrabType GetGrabType() const;
};