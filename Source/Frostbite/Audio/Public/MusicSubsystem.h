// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MusicSubsystem.generated.h"

class UAudioComponent;
class UMusicScript;
class UMetaSoundSource;

/** Subsystem for music. */
UCLASS(ClassGroup = "Music", Meta = (DisplayName = "Music Subsystem"))
class FROSTBITE_API UMusicSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogMusic, Log, All);

private:
	UPROPERTY(BlueprintGetter = GetMusicAudioComponent)
	UAudioComponent* AudioComponent;

	UPROPERTY(BlueprintGetter = GetMusicScript)
	UMusicScript* MusicScript;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Music", Meta = (Displayname = "Activate Music"))
	void ActivateMusicSubystem(UMusicSettings* Settings);
	
	UFUNCTION(BlueprintGetter, Category = "Music", Meta = (DisplayName = "Audio Component"))
	FORCEINLINE UAudioComponent* GetMusicAudioComponent() const { return AudioComponent; }

	UFUNCTION(BlueprintGetter, Category = "Music", Meta = (DisplayName = "Music Script"))
	FORCEINLINE UMusicScript* GetMusicScript() const { return MusicScript; }
};

UCLASS(BlueprintType, ClassGroup = "Music", Meta = (DisplayName = "Music Settings"))
class FROSTBITE_API UMusicSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Music", Meta = (DisplayName = "Metasound Source"))
	TSoftObjectPtr<UMetaSoundSource> MetasoundSourceAsset;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, BlueprintReadOnly, Category = "Music", Meta = (DisplayName = "Music Script",
		DisplayThumbnail))
	TSubclassOf<UMusicScript> MusicScriptClass;
};
