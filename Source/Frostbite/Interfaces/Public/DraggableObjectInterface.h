// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "DraggableObjectInterface.generated.h"

/** Interface for draggable objects. This interface can be implemented in any UObject derived class. */
UINTERFACE(Blueprintable, Meta = (DisplayName = "Draggable Object Interface", ShortToolTip = "Interface for draggable objects."))
class FROSTBITE_API UDraggableObject : public UInterface
{
	GENERATED_BODY()
};

class FROSTBITE_API IDraggableObject
{
	GENERATED_BODY()

public:
	/** Starts dragging the object.
	 *	@Param Instigator The actor that instigated the interaction.
	 *	@Return Whether the object implements a special action on drag.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Use"))
	bool BeginDrag(const AActor* Interactor);

	/** Stop dragging the object.
	 *	@Param Instigator The actor that instigated the interaction.
	 *	@Return Whether the object implements a special action on release.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractableObject", Meta = (DisplayName = "Disuse"))
	bool EndDrag(const AActor* Interactor);
};