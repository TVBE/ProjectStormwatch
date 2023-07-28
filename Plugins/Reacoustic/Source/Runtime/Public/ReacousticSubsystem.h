// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
// Written by Nino Saglia.

#pragma once

#include "ReacousticSettings.h"
#include "CoreMinimal.h"
#include "ReacousticDataTypes.h"
#include "ReacousticSoundAsset.h"
#include "Subsystems/WorldSubsystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ReacousticSubsystem.generated.h"

UCLASS()
class REACOUSTIC_API  UReacousticSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogReacousticSubsystem, Log, All)

public:

	/** The Reacoustic Sound Data Reference Map.*/
	UPROPERTY(BlueprintReadWrite, Meta = (DisplayName = "Sound Data Asset Reference Map"))	
	UReacousticSoundAssociationMap* ReacousticSoundAssociationMap {NewObject<UReacousticSoundAssociationMap>()};

	/** The internal reference of the global reacoustic settings.*/
	UPROPERTY();
	UReacousticProjectSettings* Settings;
	
	/** Returns all actors in the level of a given class that have Physics and GenerateHitEvents enabled. This is a slow operation, and should not be called often during runtime.
	 *	@ClassType Which class to look for in the world.
	 *	@return An array of actor pointers that potentially meet the conditions to be registered by Reacoustic.
	 */
	UFUNCTION()
	TArray<AActor*> GetCompatibleActorsOfClass(UClass* ClassType);

private:
	/** Array of pointers to all currently active ReacousticComponents. */
	TArray<class UReacousticComponent*> ReacousticComponents;

public:
	UReacousticSubsystem();
	virtual void PostInitProperties() override;
	void OnActorSpawned(AActor* Actor);

	
	/** Registers an component to the Reacoustic subsystem. This function is called by a component OnConstruct
	 *	@Component The component to register.
	 */
	void RegisterComponent(class UReacousticComponent* Component);

	/** Unregisters an component to the Reacoustic subsystem. This function is called by a component at EndPlay
	 *	@Component The component to register.
	 */
	void UnregisterComponent(UReacousticComponent* Component);
	
	
	/** A BP_Reacoustic component will be added to the physics actor if conditions are met.
	 *	@Actor The actor to add the component to.
	 *	@ComponentClass The reacoustic blueprint component to add.
	 */
	UFUNCTION(BlueprintCallable, Category = "ReacousticSubsystem")
	void AddBPReacousticComponentToActor(AActor* Actor, TSubclassOf<UReacousticComponent> ComponentClass, UReacousticSoundAsset* MeshSoundAsset);

	/** Gets the sound data associated with a specific mesh.*/
	UReacousticSoundAsset* GetMeshSoundAsset(const UStaticMeshComponent* StaticMeshComponent) const;
	UReacousticSoundAsset* GetSurfaceSoundAsset(const EPhysicalSurface SurfaceIn) const;

	/** Gets the matching timestamp and strength for a given normalized physics impact value.
	 *This function needs to be in the subsystem so that the it can be used in game and in editor.*/
	UFUNCTION(BlueprintCallable, Category = "ReacousticSubsystem")
	FVector2D GetTimeStampWithStrenght(UReacousticSoundAsset* SoundAsset, float ImpactValue);
	
	/** Checks whether an actor meets the conditions to be used by Reacoustic.
	 *	For this, an actor must have IsSimulatingPhysics and a StaticMeshComponent with bNotifyRigidBodyCollision set to true.
	 *	@Actor The actor to check the condition for.
	 *	@Return Whether the actor is compatible with Reacoustic or not.
	 */
	UFUNCTION(BlueprintPure, Meta = (DisplayName = "Is Reacoustic Compatible"))
	bool IsReacousticCompatible(AActor* Actor);

private:
	UFUNCTION(BlueprintCallable)
	void PopulateWorldWithBPReacousticComponents(TSubclassOf<UReacousticComponent> ComponentClass);
};




	
