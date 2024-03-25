// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BHSemanticComponent.generated.h"

USTRUCT(BlueprintType)
struct FBHSemanticGameplayTag
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FText TagName;
};

UCLASS(BlueprintType, ClassGroup = "Barrelhouse", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHSemanticComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBHSemanticComponent();

	UFUNCTION(BlueprintPure, Category = "BHSemanticComponent")
	bool IsDisplayable() const;

	UFUNCTION(BlueprintGetter)
	const FText& GetSemanticName() const;

	UFUNCTION(BlueprintGetter)
	const FText& GetSemanticDescription() const;

	UFUNCTION(BlueprintGetter)
	const UTexture2D* GetThumbnail() const;

	UFUNCTION(BlueprintGetter)
	const TArray<FBHSemanticGameplayTag>& GetSemanticTags() const;

protected:
#if WITH_EDITOR
 virtual void CheckForErrors() override;
#endif
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetSemanticName, Category = "Semantics", Meta = (DisplayName = "Name"))
	FText SemanticName;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetSemanticName, Category = "Semantics",
	Meta = (Multiline = "true", DisplayName = "Description"))
	FText SemanticDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetThumbnail, Category = "Semantics")
	UTexture2D* Thumbnail = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetSemanticTags, Category = "Tags")
	TArray<FBHSemanticGameplayTag> SemanticTags;
};
