// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class FBHPlayerCharacterComponent
{
protected:
	STORMWATCH_API class ABHPlayerCharacter* GetPlayerCharacter() const;
	STORMWATCH_API class ABHPlayerController* GetPlayerCharacterController() const;
};
