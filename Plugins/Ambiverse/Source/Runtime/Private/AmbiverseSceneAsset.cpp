// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#include "AmbiverseSceneAsset.h"
#include "AmbiverseElement.h"

#if WITH_EDITOR
void UAmbiverseSceneAsset::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.Property != nullptr)
	{
		const FName PropertyName = PropertyChangedEvent.Property->GetFName();
		
		if(PropertyName == GET_MEMBER_NAME_CHECKED(UAmbiverseSceneAsset, Elements))
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

