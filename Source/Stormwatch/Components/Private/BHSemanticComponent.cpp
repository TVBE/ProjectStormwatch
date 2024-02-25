// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHSemanticComponent.h"

UBHSemanticComponent::UBHSemanticComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UBHSemanticComponent::IsDisplayable() const
{
	return !SemanticName.IsEmpty() && IsValid(Thumbnail);
}

const FText& UBHSemanticComponent::GetSemanticName() const
{
	return SemanticName;
}

const FText& UBHSemanticComponent::GetSemanticDescription() const
{
	return SemanticDescription;
}

const UTexture2D* UBHSemanticComponent::GetThumbnail() const
{
	return Thumbnail;
}

const TArray<FBHSemanticGameplayTag>& UBHSemanticComponent::GetSemanticTags() const
{
	return SemanticTags;
}


