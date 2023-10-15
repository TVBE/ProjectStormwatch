// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHUsableObjectInterface.generated.h"

UENUM(BlueprintType)
enum class EBHUseType : uint8
{
	SinglePress			UMETA(DisplayName = "Single Press"),
	PressAndHold		UMETA(DisplayName = "Press And Hold"),
};

UINTERFACE(Blueprintable, Meta = (DisplayName = "Usable Object Interface", ShortToolTip = "Interface for usable objects."))
class STORMWATCH_API UBHUsableObject : public UInterface
{
	GENERATED_BODY()
};

class STORMWATCH_API IBHUsableObject
{
	GENERATED_BODY()

public:
	/** Begins interaction with the object.
	 *	@Param Instigator The actor that instigated the interaction.
	 *	@Return Whether the object implements an interaction.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Usable Object")
	bool BeginUse(const AActor* Interactor);

	/** Ends interaction with the object.
	 *	@Param Instigator The actor that instigated the interaction.
	 *	@Return Whether the object implements an interaction.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Usable Object")
	bool EndUse(const AActor* Interactor);

	/** Returns whether the object has a single-press interaction, or a press-and-hold interaction. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Usable Object")
	EBHUseType GetUseType() const;
};
