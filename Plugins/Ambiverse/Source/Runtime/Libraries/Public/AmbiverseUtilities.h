// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class AmbiverseUtilities
{
public:
	/**
	* This function attempts to retrieve the transform of the primary
	* audio listener from the main audio device.
	* 
	* @param[out] Transform Reference to a FTransform that will hold the listener's transform 
	* if the function succeeds.
	* @return bool Returns true if the transform was successfully retrieved and false otherwise.
	* If false, the contents of the Transform parameter are undefined.
	*/
	static bool GetListenerTransform(FTransform& Transform);

};