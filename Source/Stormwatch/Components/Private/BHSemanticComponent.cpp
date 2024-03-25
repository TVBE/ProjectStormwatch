// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHSemanticComponent.h"

#include "Misc/UObjectToken.h"

#define LOCTEXT_NAMESPACE "BHPhysicsInteractableComponent"

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

#if WITH_EDITOR
void UBHSemanticComponent::CheckForErrors()
{
	Super::CheckForErrors();
	
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}
	if (SemanticName.IsEmpty())
	{
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("ComponentName"), FText::FromString(GetName()));
		Arguments.Add(TEXT("OwnerName"), FText::FromString(Owner->GetName()));
		FMessageLog("MapCheck").Error()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::Format(LOCTEXT("MapCheck_Message_NoSemanticName",
				"{OwnerName}::{ComponentName} has no semantic name set."), Arguments)));
	}

	// TODO (TV): Enable this once thumbnails are implemented.
	// if (!Thumbnail)
	// {
	// 	FFormatNamedArguments Arguments;
	// 	Arguments.Add(TEXT("ComponentName"), FText::FromString(GetName()));
	// 	Arguments.Add(TEXT("OwnerName"), FText::FromString(Owner->GetName()));
	// 	FMessageLog("MapCheck").Error()
	// 		->AddToken(FUObjectToken::Create(this))
	// 		->AddToken(FTextToken::Create(FText::Format(LOCTEXT("MapCheck_Message_NoThumbnail",
	// 			"{OwnerName}::{ComponentName} has no thumbnail set."), Arguments)));
	// }
}
#endif


