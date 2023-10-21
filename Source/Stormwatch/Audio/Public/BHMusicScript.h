// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BHMusicScript.generated.h"

class UAudioComponent;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Music")
class UBHMusicScript : public UObject
{
	GENERATED_BODY()

public:
	/** The audio component that is used for music playback. */
	UPROPERTY(BlueprintReadOnly)
	UAudioComponent* AudioComponent;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "Music Begin Play"))
	void EventOnMusicBeginplay();
	
public:
	void Initialize();
};
