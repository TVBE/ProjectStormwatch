// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
// Written by Nino Saglia.

#pragma once

#include "Reacoustic.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReacousticDataTypes.h"
#include "ReacousticComponent.generated.h"

class ReacousticSoundDataRef_Map;


UCLASS(Abstract, Blueprintable, ClassGroup = "Reacoustic", Meta = (BlueprintSpawnableComponent))
class REACOUSTIC_API UReacousticComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** Pointer to the StaticMeshComponent of the owner of this component. */
	UPROPERTY(Transient, BlueprintGetter = GetOwnerMeshComponent, Category = "Reacoustic Component", Meta = (DisplayName = "Static Mesh Component"))
	UStaticMeshComponent* MeshComponent {nullptr};
	
	/** These variables are used to calculate the difference in distance and time between hits.
	 * Used to filter out unwanted hits.
	 */
	double DeltaHitTime;
	double DeltaLocationDistance;
	double DeltaForwardVector;
	FVector LatestLocation;
	double LatestTime;
	FVector LatestForwardVector;
	TArray<float> DeltaStateArray;
	
	/** This variable is used to choose the impact sound during a hit.*/
	UPROPERTY(BlueprintReadOnly, Category = Default, Meta = (DisplayName = "ImapctForce", AllowPrivateAccess = "true"))	
	float ImpactForce;
	

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

protected:
	/** The Reacoustic SoundData Data Asset. */
	UPROPERTY(BlueprintReadWrite, Category = "Reacoustic Component", Meta = (DisplayName = "Sound Data Asset"))
	UReacousticSoundDataAsset* ReacousticSoundDataAsset {nullptr};

	UPROPERTY(BlueprintReadWrite, Category = "Reacoustic Component", Meta = (DisplayName = "StaticMeshSoundData"))
	FReacousticSoundData MeshAudioData;

	/** The Reacoustic Sound Data Reference Map.*/
	UPROPERTY(BlueprintReadWrite, Category = "Reacoustic Component", Meta = (DisplayName = "Sound Data Asset Reference Map"))	
	UReacousticSoundDataRef_Map* UReacousticSoundDataRefMap {nullptr};

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

	UFUNCTION(BlueprintPure)
	double ReturnDeltaLocationDistance();
	
	UFUNCTION()
	void TransferData(UReacousticSoundDataAsset* SoundDataArray, UReacousticSoundDataRef_Map* ReferenceMap, FReacousticSoundData MeshSoundDataIn);

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
