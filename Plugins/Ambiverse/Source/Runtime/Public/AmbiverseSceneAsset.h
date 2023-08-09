// Copyright (c) 2023-present Tim Verberne. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AmbiverseParameterAsset.h"
#include "TimerManager.h"
#include "AmbiverseSceneAsset.generated.h"

struct FAmbiverseElement;
class UAmbiverseElement;
class UAmbiverseSubsystem;
class UAmbiverseParameterAsset;
class USoundSourcePool;

#if WITH_EDITORONLY_DATA
USTRUCT(BlueprintType)
struct FSceneDisplayData
{
	GENERATED_BODY()

	/** User friendly name for the scene. */
	UPROPERTY(EditAnywhere, Category = "Editor")
	FName Name;

	/** A description for the scene. */
	UPROPERTY(EditAnywhere, Category = "Editor", Meta = (MultiLine = "true"))
	FText Description;

	/** The color of the scene when visualized in the editor. */
	UPROPERTY(EditAnywhere, Category = "Editor")
	FColor Color {FColor::Blue};
};
#endif

UCLASS(Blueprintable, BlueprintType, ClassGroup = "Ambiverse", Meta = (DisplayName = "Ambiverse Scene",
	ShortToolTip = "A list of Ambiverse Elements that can be procedurally played."))
class AMBIVERSE_API UAmbiverseSceneAsset : public UObject
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere)
	FSceneDisplayData DisplayData;
#endif
	
	/** the procedural sound data of this scene. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Elements", Meta = (DisplayName = "Elements", TitleProperty = "ElementAsset"))
	TArray<FAmbiverseElement> Elements;
	
	/** Parameters that influence all procedural sounds in this scene. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters", Meta = (TitleProperty = "Parameter"))
	TArray<FAmbiverseParameterModifiers> Parameters;

	/** Volume multiplier for all sounds in this scene. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (DisplayName = "Volume", ClampMin = "0", ClampMax = "2", UIMin = "1", UIMax ="2"))
	float SceneVolume {1.0f};

	/** Rate multiplier for all sounds in this scene. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (DisplayName = "Density", ClampMin = "0", ClampMax = "2", UIMin = "1", UIMax ="2"))
	float SceneDensity {1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (DisplayName = "Enable Layer"))
	bool IsEnabled {true};

	/** If true, the scene has a finite lifetime and will expire when this duration is reached after becoming active. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lifetime")
	bool EnableLifetime {false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lifetime", Meta = (EditCondition = "EnableLifetime", ClampMin = "0"))
	float Lifetime {30.0f};

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};




