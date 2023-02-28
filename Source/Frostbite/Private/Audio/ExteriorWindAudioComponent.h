// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExteriorWindAudioComponent.generated.h"

class UMetaSoundSource;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (Audio), Meta = (BlueprintSpawnableComponent) )
class UExteriorWindAudioComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** The direction of the wind component, This will dictate in which direction the collision queries will be performed. Treat this value as the world rotation of this component. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "Direction"))
	FRotator WindDirection {FRotator(0, 0, 0)};

	/** The trace length for the collision queries. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "Collision Trace Length"))
	float CollisionTraceLength {3000};

private:
	/** The AudioComponent that is added to the owner of this actor to play wind audio on. */
	UPROPERTY(BlueprintGetter = GetAudioComponent, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "Audio Component"))
	UAudioComponent* AudioComponent;
	
	/** The MetaSound asset to use for the exterior wind. */
	UPROPERTY(EditAnywhere, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "MetaSound Asset"))
	TSoftObjectPtr<UMetaSoundSource> MetaSoundAsset;

	/** The last poll location of the component. */
	UPROPERTY()
	FVector LastPollLocation;

	/** The array of vectors to use for the terrain poll trace. */
	UPROPERTY()
	TArray<FVector> TerrainTraceEndVectors;

	/** The arrays of vectors to use for the wind occlusion query. */
	UPROPERTY()
	TArray<FVector> OcclusionTraceStartVectors;
	UPROPERTY()
	TArray<FVector> OcclusionTraceEndVectors;
	
public:	
	/** Sets default values for this component's properties. */
	UExteriorWindAudioComponent();
	
	/** Called every frame. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Called when a poll is performed. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "On Poll"))
	void EventOnPoll(const TArray<float>& TerrainTraceResults, const TArray<float>& OcclusionTraceResults);

	/** Sets the wind direction. */
	UFUNCTION(BlueprintCallable, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "Set Wind Direction"))
	void SetWindDirection(const FRotator& Rotation);

protected:
	/** Called when the game starts. */
	virtual void BeginPlay() override;

	/** Called when before the object is destroyed or when the game ends. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Called when the component is initialized, but before BeginPlay. */
	virtual void InitializeComponent() override;
	
	/** Returns an array of terrain trace lenghts. */
	UFUNCTION(BlueprintPure, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "Do Terrain Collision Query"))
	TArray<float> DoTerrainCollisionQuery(const FVector& Location);

	/** Returns an array of occlusion traces. */
	UFUNCTION(BlueprintPure, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "Do Occlusion Collision Query"))
	TArray<float> DoOcclusionCollisionQuery(const FVector& Location);

private:
	/** Populates the terrain trace vector array. */
	static void PopulateTerrainTraceVectors(TArray<FVector>& Array, const FRotator& Rotation, const float Radius, const float NumPoints);

	/** Populates the occlusion trace vector arrays. */
	static void PopulateOcclusionTraceVectors(TArray<FVector>& ArrayA, TArray<FVector>& ArrayB, const FRotator& Rotation, const float TraceLength, const float Spacing);

public:
	/** Returns the audio component that is used for playing wind audio. */
	UFUNCTION(BlueprintGetter, Category = "ExteriorWindAudioComponent", Meta = (DisplayName = "Audio Component"))
	FORCEINLINE UAudioComponent* GetAudioComponent() const {return AudioComponent; }
};
