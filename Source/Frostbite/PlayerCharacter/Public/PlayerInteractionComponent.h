// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne & Nino Saglia
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "PlayerPhysicsGrabComponent.h"
#include "Components/ActorComponent.h"
#include "PlayerInteractionComponent.generated.h"

class UCameraComponent;
struct FCollisionQueryParams;

/** The interaction action type. */
UENUM(BlueprintType)
enum class EInteractionActionType : uint8
{
	Primary					UMETA(DisplayName = "Primary"),
	Secondary				UMETA(DisplayName = "Secondary"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableActorFoundDelegate, AActor*, InteractableActor);

/** PlayerCharacter component that checks for interactable objects in front of the player character. */
UCLASS(Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Meta = (BlueprintSpawnableComponent,
	DisplayName = "Player Interaction Component"))
class FROSTBITE_API UPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Delegate called when the CurrentInteractableActor property is changed. */
	UPROPERTY(BlueprintAssignable, Category = "PlayerInteractionComponent|Delegates", Meta = (DisplayName = "On Interactable Actor Found"))
	FInteractableActorFoundDelegate OnInteractableActorFound;

private:
	/** The physics grab component that is used to grab actors. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Grab Component", AllowPrivateAccess = "true"))
	UPlayerPhysicsGrabComponent* GrabComponent;
	
	/** The camera component of the Player Character. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Camera", AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** The length of the initial line trace. */
	UPROPERTY(EditDefaultsOnly, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Camera Trace Length", ClampMax = "500", UIMax = "500"))
	uint16 CameraTraceLength {300};

	/** The radius of the interactable object multi sphere trace. */
	UPROPERTY(EditDefaultsOnly, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Object Trace Radius", ClampMax = "500", UIMax = "500"))
	uint16 ObjectTraceRadius {50};

	/** The location of the camera. */
	UPROPERTY()
	FVector CameraLocation;
	
	/** The hit result for the initial visibility line trace collision query performed from the camera. */
	UPROPERTY()
	FHitResult CameraTraceHitResult {FHitResult()};

	/** The hit result for the occlusion line trace collision query performed from the camera. */
	UPROPERTY()
	FHitResult OcclusionTraceHitResult {FHitResult()};

	/** The array of hit results for the interactable object multi sphere trace. */
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TArray<FHitResult> ObjectTraceHitResults;

	/** The actor that currently can be interacted with. Will be a nullptr if no object can be interacted with at the moment. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Current Interactable Actor", AllowPrivateAccess = "true"))
	AActor* CurrentInteractableActor;

	/** The actor that could be interacted with during the previous frame. */
	UPROPERTY()
	AActor* PreviousInteractableActor;

	/** The actor that is currently being interacted with. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Current Interacting Actor", AllowPrivateAccess = "true"))
	AActor* CurrentInteractingActor;

	/** The actor that is currently being used. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Actor In Use", AllowPrivateAccess = "true"))
	AActor* ActorInUse;

	/** The collision query data for the camera trace. */
	FCollisionQueryParams CameraTraceQueryParams;
	
	/** The timer for the update function. */
	UPROPERTY()
	FTimerHandle UpdateTimerHandle;

	/** When true, we will draw debug visualisation to the screen for every collision query. */
	UPROPERTY(EditAnywhere, Category = "PlayerInteractionComponent|Debugging", Meta = (DisplayName = "Enable Debug Visualisation"))
	bool IsDebugVisEnabled {false};

	/** For setting the parameters of the playerPhysicsGrabComponent. */
	UPROPERTY(EditAnywhere, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Settings for player physics grab compoent"))
	TSoftObjectPtr<UPlayerPhysicsGrabConfiguration> PlayerPhysicsGrabConfiguration;

public:	
	UPlayerInteractionComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/** begins interaction with an object if possible.
	*	@Return Whether an interaction was successfully started.
	*/
	UFUNCTION(BlueprintCallable, Category = "PlayerInteractionComponent", Meta = (DisplayName = "End Interaction"))
	UObject* BeginInteraction(const EInteractionActionType Type);

	/** Ends interaction with an object if possible.
	 *	@Return The object the interaction was ended with.
	 */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteractionComponent", Meta = (DisplayName = "End Interaction"))
	UObject* EndInteraction(const EInteractionActionType Type);

	/** Releases and actor that is currently being used. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Release Actor In Use"))
	void ReleaseActorInUse();

	/** Checks if an actor or one of its components implements the IInteractableObject interface.
	*	Returns the first UObject that implements the interface that it finds. */
	UFUNCTION(BlueprintPure, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Find Interactable Object"))
	UObject* FindInteractableObject(AActor* Actor) const;

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;
	 
	/** Checks if any interactable objects are in front of the player. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Check For Interactable Objects", BlueprintProtected))
	AActor* CheckForInteractableActor();

private:
	/** Performs a line trace from the camera. */
	UFUNCTION()
	void PerformTraceFromCamera(FHitResult& HitResult);

	/** Performs a multi sphere trace for objects that respond to the interactable trace channel. */
	UFUNCTION()
	void PerformInteractableObjectTrace(TArray<FHitResult>& Array, const FHitResult& HitResult);

	/** Returns the closest object to the specified hit hit result from an array of hit results. */
	UFUNCTION()
	static AActor* GetClosestObject(const TArray<FHitResult>& Array, const FHitResult& HitResult);

	/** Checks whether an actor is occluded. Is used to prevent the interaction component from highlighting objects behind walls or other objects. */
	UFUNCTION()
	bool IsActorOccluded(const AActor* Actor);
	
	/** Tries to find a component that implements the IInteractableObject interface in a specified actor.*/
	UFUNCTION()
	UActorComponent* FindInteractableComponent(const AActor* Actor) const;

public:
	/** Returns the object the interaction component is currently interacting with. */
	UFUNCTION(BlueprintPure, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Current Interaction Object"))
	FORCEINLINE AActor* GetCurrentInteractingActor() const { return CurrentInteractingActor; }

	/** Returns whether there is an object in front of the player that can be interacted with. */
	UFUNCTION(BlueprintPure, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Can Interact"))
	FORCEINLINE bool CanInteract() const { return static_cast<bool>(CurrentInteractableActor); }

	/** Returns a pointer to an object that is currently available for interaction. Returns a nullptr if no interactable object is in front of the player. */
	UFUNCTION(BlueprintPure, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Get Current Interactable Actor"))
	FORCEINLINE AActor* GetCurrentInteractableActor() const { return CurrentInteractableActor; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Begin Interaction"))
	void EventBeginInteraction(const EInteractionActionType Type, const UObject* Object);
    
	UFUNCTION(BlueprintNativeEvent, Category = "PlayerInteractionComponent", Meta = (DisplayName = "End Interaction"))
	void EventEndInteraction(const EInteractionActionType Type, const UObject* Object);
};

