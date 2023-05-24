// Copyright 2023 Nino Saglia & Tim Verberne

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReacousticDataTypes.h"
#include "Components/AudioComponent.h"
#include "ReacousticSubsystem.h"
#include "ReacousticComponent.generated.h"

class ReacousticSoundDataRef_Map;


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(Blueprintable) )
class REACOUSTIC_API UReacousticComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** Pointer to the StaticMeshComponent of the owner of this component. */
	UPROPERTY(BlueprintGetter = GetOwnerMeshComponent, Category = Reacoustic, Meta = (DisplayName = "StaticMeshComponent"))
	UStaticMeshComponent* MeshComponent {nullptr};

	UPROPERTY(BlueprintReadWrite, Category = Default, Meta = (DisplayName = "AudioComponent", AllowPrivateAccess = "true"))
	UAudioComponent* AudioComponent;

	/** The Reacoustic SoundData Data Asset. */
	UPROPERTY(BlueprintReadWrite, Category = Default, Meta = (DisplayName = "Sound Data Asset", AllowPrivateAccess = "true"))
	UReacousticSoundDataAsset* ReacousticSoundDataAsset {nullptr};

	UPROPERTY(BlueprintReadWrite, Category = Default, Meta = (DisplayName = "StaticMeshSoundData", AllowPrivateAccess = "true"))
	FReacousticSoundData MeshAudioData;

	/** The Reacoustic Sound Data Reference Map.*/
	UPROPERTY(BlueprintReadWrite, Category = Default, Meta = (DisplayName = "Sound Data Asset Reference Map", AllowPrivateAccess = "true"))	
	UReacousticSoundDataRef_Map* UReacousticSoundDataRefMap {nullptr};
	
	/** These variables are used to calculate the difference in distance and time between hits.
	 * Used to filter out unwanted hits.
	 */
	double DeltaHitTime;
	double DeltaLocationDistance;
	double DeltaDirectionVector;
	
	FVector LatestForwardVector;
	FVector LatestRightVector;
	FVector LatestUptVector;
	
	FVector LatestLocation;
	double LatestTime;

	
	TArray<float> DeltaStateArray;
	

	/** Array used to store the latest hit values so that we can prevent multiple triggers of the same sound.*/
	UPROPERTY(BlueprintReadOnly, Category = Default, Meta = (DisplayName = "LatestHitResults", AllowPrivateAccess = "true"))
	TArray<float> LatestSoundTimings;


protected:
	/** This variable is used to choose the impact sound during a hit.*/
	UPROPERTY(BlueprintReadOnly, Category = Default, Meta = (DisplayName = "Impact Force"))	
	float ImpactForce;

private:
	/** Utility function to calculate the sum of elements in an array of floats */
	UFUNCTION()
	static float GetArraySum(TArray<float> Array)
	{
		float Sum = 0.0;
		for (float Element : Array)
		{
			Sum += Element;
		}
		return Sum;
	}
	
public:	
	UReacousticComponent();	
	/** Callback function for the OnHit event delegate of a physics enabled static mesh component.
	 *	@HitComp The component that was hit.
	 *	@OtherActor The other actor that was hit by the component.
	 *	@OtherComp	The other actor's root component.
	 *	@NormalImpulse	The normal impulse of the hit.
	 *	@Hit A FHitResult struct of the hit.
	 */
	UFUNCTION()
	void HandleOnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	

	UFUNCTION(BlueprintNativeEvent, Category = Default, Meta = (DisplayName = "Initialize"))
	void Initialize(USoundBase* SoundBase = nullptr);
	
	UFUNCTION(BlueprintNativeEvent, Category = Default, Meta = (DisplayName = "Trigger manual hit"))
	void TriggerManualHit(float HitStrength);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	/** Returns the MeshComponent of the owner. */
	UFUNCTION(BlueprintGetter, Category = Reacoustic, Meta = (DisplayName = "Get Owner StaticMeshComponent"))
	FORCEINLINE UStaticMeshComponent* GetOwnerMeshComponent() const {return MeshComponent; }



	/** Get the time interval between hits */
	UFUNCTION(BlueprintPure)
	double ReturnDeltaTime();
	
	/** Get the location interval between hits */
	UFUNCTION(BlueprintPure)
	double ReturnDeltaLocationDistance();
	
	UFUNCTION()
	void TransferData(UReacousticSoundDataAsset* SoundDataArray, UReacousticSoundDataRef_Map* ReferenceMap, FReacousticSoundData MeshSoundDataIn);

	/** Caluclates the integer entry number in FReacousticSoundData that matches the impact value.*/
	UFUNCTION(BlueprintCallable)
	int CalcluateSoundDataEntry(FReacousticSoundData SoundData, float ImpactValue);
	
	
	
protected:
	virtual void BeginPlay() override;
	virtual void OnComponentCreated() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TriggerManualHit_Implementation(float HitStrength);

	UFUNCTION(BlueprintCallable, Category = "Reacoustic", Meta = (DisplayName = "Get Scaled Impact Value"))
	static float GetScaledImpactValue(const FVector& NormalImpulse, const UPrimitiveComponent* HitComponent, const AActor* OtherActor);

	UFUNCTION(BlueprintCallable, Category = "Reacoustic", Meta = (DisplayName = "Get Surface Hit Sound"))
	static FReacousticSoundData GetSurfaceHitSoundX(const AActor* Actor, const UPhysicalMaterial* PhysicalMaterial);
	
};


