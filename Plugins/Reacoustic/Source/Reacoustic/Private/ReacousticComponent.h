// Copyright 2023 Nino Saglia & Tim Verberne

#pragma once

#include "Reacoustic.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReacousticDataTypes.h"
#include "ReacousticComponent.generated.h"

class ReacousticSoundDataRef_Map;


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(Blueprintable) )
class UReacousticComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** Pointer to the StaticMeshComponent of the owner of this component. */
	UPROPERTY(BlueprintGetter = GetOwnerMeshComponent, Category = Reacoustic, Meta = (DisplayName = "StaticMeshComponent"))
	UStaticMeshComponent* MeshComponent {nullptr};

	/** The Reacoustic SoundData Data Asset. */
	UPROPERTY(BlueprintReadWrite, Category = Default, Meta = (DisplayName = "Sound Data Asset", AllowPrivateAccess = "true"))
	UReacousticSoundDataAsset* ReacousticSoundDataAsset {nullptr};

	UPROPERTY(BlueprintReadWrite, Category = Default, Meta = (DisplayName = "StaticMeshSoundData", AllowPrivateAccess = "true"))
	FReacousticSoundData MeshAudioData;

	/** The Reacoustic Sound Data Reference Map.*/
	UPROPERTY(BlueprintReadWrite, Category = Default, Meta = (DisplayName = "Sound Data Asset Reference Map", AllowPrivateAccess = "true"))	
	UReacousticSoundDataRef_Map* UReacousticSoundDataRefMap {nullptr};

	
	
public:	
	// Sets default values for this component's properties
	UReacousticComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// called after the component has been fully created and registered, but before it has been initialized and is ready for use.
	UFUNCTION (BlueprintInternalUseOnly)
	virtual void OnComponentCreated() override;
	
	// Called before the object is destructed
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Callback function for the OnHit event delegate of a physics enabled static mesh component.
	 *	@HitComp The component that was hit.
	 *	@OtherActor The other actor that was hit by the component.
	 *	@OtherComp	The other actor's root component.
	 *	@NormalImpulse	The normal impulse of the hit.
	 *	@Hit A FHitResult struct of the hit.
	 */
	UFUNCTION()
	void HandleOnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	/** Returns the MeshComponent of the owner. */
	UFUNCTION(BlueprintGetter, Category = Reacoustic, Meta = (DisplayName = "Get Owner StaticMeshComponent"))
	FORCEINLINE UStaticMeshComponent* GetOwnerMeshComponent() const {return MeshComponent; }

	/** Returns the impact force*/
	UFUNCTION(BlueprintPure)
	float ReturnImpactForce(UPrimitiveComponent* HitComp, AActor* OtherActor,
												 UPrimitiveComponent* OtherComp, FVector NormalImpulse,
												 const FHitResult& Hit);

	UFUNCTION()
	void TransferData(UReacousticSoundDataAsset* SoundDataArray, UReacousticSoundDataRef_Map* ReferenceMap, FReacousticSoundData MeshSoundDataIn);
};
