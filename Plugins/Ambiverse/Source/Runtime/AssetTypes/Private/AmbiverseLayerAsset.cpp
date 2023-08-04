// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseLayerAsset.h"
#include "AmbiverseElement.h"

DEFINE_LOG_CATEGORY_CLASS(UAmbiverseLayerAsset, LogAmbiverseLayer);

#if WITH_EDITOR
void UAmbiverseLayerAsset::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.Property != nullptr)
	{
		const FName PropertyName = PropertyChangedEvent.Property->GetFName();
		
		if(PropertyName == GET_MEMBER_NAME_CHECKED(UAmbiverseLayerAsset, Elements))
		{
			for(FAmbiverseElementRuntimeData& Element : Elements)
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

