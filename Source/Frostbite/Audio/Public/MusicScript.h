// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

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
	UPROPERTY(BlueprintReadOnly, Category = "Music", Meta = (DisplayName = "Audio Component"))
	UAudioComponent* AudioComponent;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Music", Meta = (DisplayName = "Music Begin Play"))
	void EventOnMusicBeginplay();
	
public:
	void Initialize();
};
