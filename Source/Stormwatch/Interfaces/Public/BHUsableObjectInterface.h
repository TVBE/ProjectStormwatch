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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHUsableObject")
	bool Use(const AActor* Instigator);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHUsableObject")
	void Release(const AActor* Instigator);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "BHUsableObject")
	EBHUseType GetUseType() const;
};
