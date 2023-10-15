// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MusicSubsystem.generated.h"

class UAudioComponent;
class UBHMusicScript;
class UMetaSoundSource;

/** Subsystem for music. */
UCLASS(ClassGroup = "Music")
class STORMWATCH_API UBHMusicSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogMusic, Log, All);

private:
	UPROPERTY(BlueprintGetter = GetMusicAudioComponent)
	UAudioComponent* AudioComponent;

	UPROPERTY(BlueprintGetter = GetMusicScript)
	UBHMusicScript* MusicScript;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Meta = (Displayname = "Activate Music"))
	void ActivateMusicSubystem(UMusicSettings* Settings);
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE UAudioComponent* GetMusicAudioComponent() const { return AudioComponent; }

	UFUNCTION(BlueprintGetter)
	FORCEINLINE UBHMusicScript* GetMusicScript() const { return MusicScript; }
};

UCLASS(BlueprintType, ClassGroup = "Music", Meta = (DisplayName = "Music Settings"))
class STORMWATCH_API UMusicSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Music", Meta = (DisplayName = "Metasound Source"))
	TSoftObjectPtr<UMetaSoundSource> MetasoundSourceAsset;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, BlueprintReadOnly, Category = "Music", Meta = (DisplayName = "Music Script",
		DisplayThumbnail))
	TSubclassOf<UBHMusicScript> MusicScriptClass;
};
