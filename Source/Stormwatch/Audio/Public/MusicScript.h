// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MusicScript.generated.h"

class UAudioComponent;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Music", Meta = (DisplayName = "Music Script",
	ShortToolTip = "Base class for persistent music script."))
class UMusicScript : public UObject
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
