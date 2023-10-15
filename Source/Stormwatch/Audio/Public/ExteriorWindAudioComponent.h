// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExteriorWindAudioComponent.generated.h"

class UMetaSoundSource;

UENUM()
enum class EBHTraceCardinalDirection : uint8
{
	North				UMETA(DisplayName = "North"),
	East				UMETA(DisplayName = "East"),
	South				UMETA(DisplayName = "South"),
	West				UMETA(DisplayName = "West"),
};

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Audio")
class STORMWATCH_API UBHExteriorWindAudioComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** The direction of the wind component, This will dictate in which direction the collision queries will be performed. Treat this value as the world rotation of this component. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Meta = (DisplayName = "Direction"))
	FRotator WindDirection = FRotator(0, 0, 0);

	/** The trace length for the collision queries. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CollisionTraceLength = 3000;

protected:
	/** The number of frames to use for a single temporal collision query.
	 *Currently we do not keep the framerate of the system in count, so keep this number below 10 to keep the query responsive. */
	UPROPERTY(EditAnywhere, Category = "Temporal Geometry Query", Meta = (DisplayName = "Temporal Geomtry Query Length",
		ClampMin = "0", ClampMax = "16", UIMin = "0", UIMax = "16"))
	uint8 TemporalTraceLength = 8;

	/** Defines the angle increment per trace frame. */
	UPROPERTY(EditAnywhere, Category = "Temporal Geometry Query",
		Meta = (ClampMin = "0", ClampMax = "20", UIMin = "0", UIMax = "20"))
	float TemporalTracePitchIncrement = 10.0f;

	/** Defines the pitch offset of the entire temporal geometry query. */
	UPROPERTY(EditAnywhere, Category = "Temporal Geometry Query",
		Meta = (ClampMin = "-45", ClampMax = "45", UIMin = "-45", UIMax = "45"))
	float TemporalTracePitchOffset = -20.0f;

private:
	/** When true, the component is currently performing a temporal terrain query. */
	UPROPERTY()
	bool IsQueryingGeometry = false;

	/** When true, we assume another query is queued after the current temporal query. */
	UPROPERTY()
	bool IsGeometryQueryQueued = false;

	/** Array of hit results from a temporal geometry query.  */
	UPROPERTY()
	TArray<FHitResult> GeometryQueryResults;

	/** The current frame of the temporal geometry query. */
	UPROPERTY()
	uint8 CurrentTemporalTraceFrame = 0;

	/** The location from a temporal query is performed. */
	UPROPERTY()
	FVector TemporalQueryOrigin;
	
	/** The AudioComponent that is added to the owner of this actor to play wind audio on. */
	UPROPERTY(BlueprintGetter = GetAudioComponent)
	UAudioComponent* AudioComponent;
	
	/** The MetaSound asset to use for the exterior wind. */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UMetaSoundSource> MetaSoundAsset;

	/** The last poll location of the component. */
	UPROPERTY()
	FVector LastPollLocation;

	/** The array of vectors to use for the terrain poll trace. */
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TArray<FVector> GeometryTraceVectors;

	/** The arrays of vectors to use for the wind occlusion query. */
	UPROPERTY()
	TArray<FVector> OcclusionTraceStartVectors;
	UPROPERTY()
	TArray<FVector> OcclusionTraceEndVectors;

#if WITH_EDITORONLY_DATA
	/** If enabled, the component will visualize the traces. */
	UPROPERTY(EditAnywhere, Category = "Editor", Meta = (DisplayName = "Enable Trace Visualisation"))
	bool IsTraceVisEnabled = false;
#endif

public:	
	/** Sets default values for this component's properties. */
	UBHExteriorWindAudioComponent();
	
	/** Called every frame. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/** Sets the wind direction. */
	UFUNCTION(BlueprintCallable)
	void SetWindDirection(const FRotator& Rotation);

protected:
	/** Called when the game starts. */
	virtual void BeginPlay() override;

	/** Called when before the object is destroyed or when the game ends. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Called when the component is initialized, but before BeginPlay. */
	virtual void InitializeComponent() override;
	
	/** Returns an array of terrain trace lenghts. */
	UFUNCTION(BlueprintCallable)
	TArray<float> DoTerrainCollisionQuery(const FVector& Location);

	/** Returns an array of occlusion traces. */
	UFUNCTION(BlueprintCallable)
	TArray<float> DoOcclusionCollisionQuery(const FVector& Location);

	/** Returns the average of a float array. */
	UFUNCTION(BlueprintPure)
	float GetAverageOfFloatArray(const TArray<float>& Array) const;

	/** Returns the average trace length of the last geometry query in a relative cardinal direction. */
	UFUNCTION(BlueprintPure)
	float GetAverageTraceLengthInCardinalDirection(const EBHTraceCardinalDirection Direction);

private:
	/** Begins a temporal geometry query. */
	void BeginTemporalGeometryQuery();

	/** Performs a single temporal geometry query frame. */
	void UpdateGeometryQuery();

	/** Resets the geometry trace result array. */
	void ResetGeometryQueryResults();
	
	/** Populate a TArray of FAzimuthVector with vectors representing points on a circle that are rotated and adjusted in elevation.
	* @param Array The TArray that will be populated.
	* @param Rotation The rotation applied to the vectors.
	* @param Radius The radius of the circle.
	* @param NumPoints The number of points to generate on the circle.
	* @param TemporalFrames The number of frames over which to distribute the generated points.
	* @param PitchIncrement The increment applied to the pitch angle at each step.
	* @param PitchOffset The offset applied to the pitch angle. */
	static void PopulateGeometryTraceVectors(TArray<FVector>& Array, const FRotator& Rotation, const float Radius,
		const float NumPoints, const uint8 TemporalFrames, const float PitchIncrement, const float PitchOffset);

	/** Sorts the geometry trace vector array depending on yaw.
	 *	This way, we can easily access all vectors in a certain direction by index.*/
	static void SortTraceVectorsByYaw(TArray<TPair<FVector, double>>& TraceVectors);

	/** Populates the occlusion trace vector arrays. */
	static void PopulateOcclusionTraceVectors(TArray<FVector>& ArrayA, TArray<FVector>& ArrayB, const FRotator& Rotation, const float TraceLength, const float Spacing);

protected:
	/** Called when a poll is performed. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Events", Meta = (DisplayName = "On Occlusion Poll"))
	void EventOnOcclusionPoll(const TArray<float>& OcclusionTraceResults);

	/** Called when a temporal geometry query is completed.*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "On Temporal Geometry Query Finished"))
	void EventOnGeometryQueryFinished(const TArray<FHitResult>& GeometryTraceResults);

	/** Called when the wind direction is updated. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "On Wind Direction Changed"))
	void EventOnWindDirectionChanged(const FRotator& Rotation);
	
public:
	/** Returns the audio component that is used for playing wind audio. */
	UFUNCTION(BlueprintGetter)
	FORCEINLINE UAudioComponent* GetAudioComponent() const {return AudioComponent; }

	/** Returns whether the component is currently performing a temporal geometry query. */
	UFUNCTION(BlueprintGetter, Category = "Temporal Geometry Query")
	FORCEINLINE bool GetIsQueryingGeometry() const { return IsQueryingGeometry; }

	/** Returns an array of hit results from the most recent geometry query.
	 *	Be careful when calling this function while the component is performing a temporal query,
	 *	as this means that the array is not fully populated yet. In this case, we return an empty array. */
	UFUNCTION(BlueprintPure, Category = "Temporal Geometry Query")
	TArray<FHitResult> GetGeometryQueryResults() const;




};
