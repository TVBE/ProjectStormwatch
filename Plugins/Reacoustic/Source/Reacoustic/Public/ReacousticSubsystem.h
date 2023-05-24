// Copyright (c) 2022-present Nino Saglia. All Rights Reserved.
// Written by Nino Saglia.

#pragma once

#include "Reacoustic.h"
#include "ReacousticSettings.h"
#include "CoreMinimal.h"
#include "ReacousticDataTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "ReacousticSubsystem.generated.h"

UCLASS()
class REACOUSTIC_API  UReacousticSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	/** Array of pointers to all currently active ReacousticComponents. */
	TArray<class UReacousticComponent*> ReacousticComponents;


public:
	/** The Reacoustic SoundData Data Asset. */
	UPROPERTY(BlueprintReadWrite, Category = Default, Meta = (DisplayName = "Sound Data array Asset"))
	UReacousticSoundDataAsset* ReacousticSoundDataAsset {NewObject<UReacousticSoundDataAsset>()};

	/** The Reacoustic Sound Data Reference Map.*/
	UPROPERTY(BlueprintReadWrite, Category = Default, Meta = (DisplayName = "Sound Data Asset Reference Map"))	
	UReacousticSoundDataRef_Map* UReacousticSoundDataRefMap {NewObject<UReacousticSoundDataRef_Map>()};

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
	void AddBPReacousticComponentToActor(AActor* Actor, TSubclassOf<UReacousticComponent> ComponentClass, FReacousticSoundData MeshSoundData);

	
	/** Checks whether an actor meets the conditions to be used by Reacoustic.
	 *	For this, an actor must have IsSimulatingPhysics and a StaticMeshComponent with bNotifyRigidBodyCollision set to true.
	 *	@Actor The actor to check the condition for.
	 *	@Return Whether the actor is compatible with Reacoustic or not.
	 */
	UFUNCTION(BlueprintPure, Category = Reacoustic, Meta = (DisplayName = "Is Reacoustic Compatible"))
	bool IsReacousticCompatible(AActor* Actor);

private:

	/** The internal reference of the global reacoustic settings.*/
	UPROPERTY();
	UReacousticProjectSettings* Settings;
	
	/** Returns all actors in the level of a given class that have Physics and GenerateHitEvents enabled. This is a slow operation, and should not be called often during runtime.
	 *	@ClassType Which class to look for in the world.
	 *	@return An array of actor pointers that potentially meet the conditions to be registered by Reacoustic.
	 */
	UFUNCTION()
	TArray<AActor*> GetCompatibleActorsOfClass(UClass* ClassType);
	
	UFUNCTION(BlueprintCallable, Category = Reacoustic)
	void PopulateWorldWithBPReacousticComponents(TSubclassOf<UReacousticComponent> ComponentClass);
};




	
