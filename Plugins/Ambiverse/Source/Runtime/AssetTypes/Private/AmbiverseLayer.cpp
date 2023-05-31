// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseLayer.h"
#include "AmbiverseParameterManager.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseLayer, LogAmbiverseLayer);

void UAmbiverseLayer::InitializeLayer()
{
	/** We check for invalid elements first and remove them from the layer.
	 *	This way, we don't have to check the validity of an element in the array while the layer is active. */
	Elements.RemoveAll([](const FAmbiverseProceduralElement& Element){ return !Element.IsValid(); });
	
	for (FAmbiverseProceduralElement& Element : Elements)
	{
			Element.RandomizeIntervalInRange();
	}
}

#if WITH_EDITOR
void UAmbiverseLayer::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.Property != nullptr)
	{
		const FName PropertyName = PropertyChangedEvent.Property->GetFName();
		
		if(PropertyName == GET_MEMBER_NAME_CHECKED(UAmbiverseLayer, Elements))
		{
			for(FAmbiverseProceduralElement& Element : Elements)
			{
				if(Element.IntervalRange.X > Element.IntervalRange.Y)
				{
					Element.IntervalRange.X = Element.IntervalRange.Y;
				}
			}
		}
	}
}
#endif

