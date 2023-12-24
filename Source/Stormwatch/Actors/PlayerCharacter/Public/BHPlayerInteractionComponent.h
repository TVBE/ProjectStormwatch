// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "BHPlayerInteractionComponent.generated.h"

UENUM(BlueprintType)
enum class EBHInteractionType : uint8
{
	Usable					UMETA(DisplayName = "Usable"),
	Grabbable				UMETA(DisplayName = "Grabbable"),
	Draggable				UMETA(DisplayName = "Draggable")
};

UENUM(BlueprintType)
enum class EBHInteractionActionType : uint8
{
	Primary					UMETA(DisplayName = "Primary", ToolTip = "The primary action is used to 'use' actors."),
	Secondary				UMETA(DisplayName = "Secondary", ToolTip = "The secondary action is used to grab actors."),
	Inventory				UMETA(DisplayName = "Inventory", ToolTip = "The inventory action is used to add or take objects from the inventory.")
};

USTRUCT(BlueprintType)
struct FBHInteractableObjectData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	UObject* Object = nullptr;

	UPROPERTY(BlueprintReadOnly)
	EBHInteractionType InteractionType = EBHInteractionType::Grabbable;

	FBHInteractableObjectData()
	{
	}

	FBHInteractableObjectData(UObject* InObject, EBHInteractionType InInteractionType)
		: Object(InObject)
		, InteractionType(InInteractionType)
	{
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeginInteraction, EBHInteractionType, Type, USceneComponent*, Component);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEndInteraction, EBHInteractionType, Type, USceneComponent*, Component);

UCLASS(Blueprintable, BlueprintType, ClassGroup = "BHPlayerCharacter")
class STORMWATCH_API UBHPlayerInteractionComponent : public UBHPlayerCharacterComponent
{
	GENERATED_BODY()

public:
		UBHPlayerInteractionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Returns the object the interaction component is currently interacting with. */
	UFUNCTION(BlueprintPure)
	AActor* GetCurrentInteractingActor() const { return CurrentInteractingActor; }

	/** Returns whether the interaction component is currently performing a tertiary interaction or not. */
	UFUNCTION(BlueprintGetter)
	bool GetIsTertiaryInteractionActive() const { return bTertiaryInteractionActive; }

	/** Returns whether there is an object in front of the player that can be interacted with. */
	UFUNCTION(BlueprintPure)
	bool CanInteract() const { return static_cast<bool>(CurrentInteractableActor); }

	/** Returns a pointer to an object that is currently available for interaction. Returns a nullptr if no interactable object is in front of the player. */
	UFUNCTION(BlueprintPure)
	AActor* GetCurrentInteractableActor() const { return CurrentInteractableActor; }

	/** Returns data about all interactable objects in an actor. This could be the actor itself as well. */
	UFUNCTION(BlueprintGetter)
	TArray<FBHInteractableObjectData> GetCurrentInteractableObjects() const { return CurrentInteractableObjects; }

	/** Returns data about the the closest interactable object */
	UFUNCTION(BlueprintCallable)
	bool GetClosestInteractableObject(FBHInteractableObjectData& InteractableObjectData)
	{
		if (!CurrentInteractableActor) { return false; }

		InteractableObjectData = ClosestInteractableObject;
		return true;
	}

	/** Returns the most recent camera trace result. */
	UFUNCTION(BlueprintPure)
	FHitResult GetCameraTraceHitResult() const { return CameraTraceHitResult; }

	/** Begins the primary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "Actions", 
			  Meta = (DisplayName = "Begin Primary Interaction", BlueprintProtected))
	void BeginPrimaryInteraction();

	/** Ends the primary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "Actions", 
			  Meta = (DisplayName = "End Primary Interaction", BlueprintProtected))
	void EndPrimaryInteraction();

	/** Begins the secondary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "Actions", 
			  Meta = (DisplayName = "Begin Secondary Interaction", BlueprintProtected))
	void BeginSecondaryInteraction();

	/** Ends the secondary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "Actions", 
			  Meta = (DisplayName = "End Secondary Interaction", BlueprintProtected))
	void EndSecondaryInteraction();

	/** Begins the tertiary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "Actions", 
			  Meta = (DisplayName = "Begin Tertiary Interaction", BlueprintProtected))
	void BeginTertiaryInteraction();

	/** Ends the tertiary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "Actions", 
			  Meta = (DisplayName = "End Tertiary Interaction", BlueprintProtected))
	void EndTertiaryInteraction();

	/** Begins the inventory action. */
	UFUNCTION(BlueprintCallable, Category = "Actions", 
			  Meta = (DisplayName = "Begin Inventory Interaction", BlueprintProtected))
	void BeginInventoryInteraction();

	/** Ends the inventory action. */
	UFUNCTION(BlueprintCallable, Category = "Actions", 
			  Meta = (DisplayName = "End Inventory Interaction", BlueprintProtected))
	void EndInventoryInteraction();

	/** Handles scroll input. */
	UFUNCTION(BlueprintCallable, Category = "Actions", 
			  Meta = (DisplayName = "Add Scroll Input", BlueprintProtected))
	void AddScrollInput(const float Input);

	/** Adds pitch input to the interaction component. */
	UFUNCTION()
	void AddPitchInput(const float Input);

	/** Adds yaw input to the interaction component. */
	UFUNCTION()
	void AddYawInput(const float Input);

	UFUNCTION(BlueprintPure, Category = "Interaction")
	class UBHPlayerInteractionComponent* GetCurrentlyInteractingComponent() const;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnBeginInteraction OnBeginInteraction;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnEndInteraction OnEndInteraction;
	
	/** The length of the initial line trace. */
	UPROPERTY(EditDefaultsOnly, Category = "PlayerInteraction", 
			  Meta = (DisplayName = "Camera Trace Length", ClampMax = "500", UIMax = "500"))
	uint16 CameraTraceLength = 300;

	/** The radius of the interactable object multi sphere trace. */
	UPROPERTY(EditDefaultsOnly, Category = "PlayerInteraction", 
			  Meta = (DisplayName = "Object Trace Radius", ClampMax = "500", UIMax = "500"))
	uint16 ObjectTraceRadius = 50;

private:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginPlay() override;

	/** Checks if any interactable objects are in front of the player. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Check For Interactable Objects", BlueprintProtected))
	AActor* CheckForInteractableActor();
	
	/** Performs a line trace from the camera. */
	UFUNCTION()
	void PerformTraceFromCamera(FHitResult& HitResult);

	/** Performs a multi sphere trace for objects that respond to the interactable trace channel. */
	UFUNCTION()
	void PerformInteractableObjectTrace(TArray<FHitResult>& Array, const FHitResult& HitResult);

	/** Returns the closest object to the specified hit hit result from an array of hit results. */
	UFUNCTION()
	static AActor* GetClosestActor(const TArray<FHitResult>& Array, const FHitResult& HitResult);

	/** Checks if an actor or one of its components implements the IInteractableObject interface.
	 *	Returns the first UObject that implements the interface that it finds. */
	template <typename TInterface>
	UObject* FindInteractableObject(AActor* Actor) const;

	/** Tries to find a component that implements the IInteractableObject interface in a specified actor.*/
	template <typename TInterface>
	UActorComponent* FindInteractableComponent(const AActor* Actor) const;

	/** Checks if an actor or one of its components implements the IInteractableObject interface.
	*	Returns all UObjects that implements the interface that it finds. */
	template <typename TInterface>
	TArray<UObject*> FindInteractableObjects(AActor* Actor) const;

	template <class TInterface>
	void AddInteractableObjectsOfType(AActor* Actor, EBHInteractionType InteractionType);

	/** Updates the current interactable actor data. */
	UFUNCTION()
	void UpdateInteractableObjectData(AActor* NewInteractableActor);

	/** Checks whether an actor is occluded. Is used to prevent the interaction component from highlighting objects behind walls or other objects. */
	UFUNCTION()
	bool IsActorOccluded(const AActor* Actor);

	/** Converts a UObject pointer to an AActor pointer.
	 *	Either by checking if the UObject is an AActor,
	 *	or by casting the UObject to an UActorComponent and retrieving the owner from the component. */
	UFUNCTION()
	AActor* GetActorFromObject(UObject* Object) const;

	/** Returns the closest USceneComponent to the player's look-at location. */
	bool GetClosestObjectToLocation(FBHInteractableObjectData& OutInteractableObjectData, const FVector& Location, TArray<FBHInteractableObjectData> Objects);
	
	/** If true, the interaction component is currently performing a tertiary interaction. */
	UPROPERTY(BlueprintGetter = GetIsTertiaryInteractionActive)
	bool bTertiaryInteractionActive = false;

	/** The hit result for the initial visibility line trace collision query performed from the camera. */
	UPROPERTY()
	FHitResult CameraTraceHitResult = FHitResult();

	/** The hit result for the occlusion line trace collision query performed from the camera. */
	UPROPERTY()
	FHitResult OcclusionTraceHitResult = FHitResult();

	/** The offset used for the occlusion trace. Prevents the occlusion trace hitting the actor underneath
	 *	the interactable object that the occlusion trace is performed for. */
	UPROPERTY()
	FVector OcclusionOffset = FVector(0, 0, 5);

	/** The array of hit results for the interactable object multi sphere trace. */
	UPROPERTY()
	TArray<FHitResult> ObjectTraceHitResults;

	/** The actor that currently can be interacted with. Will be a nullptr if no object can be interacted with at the moment. */
	UPROPERTY(BlueprintGetter = GetCurrentInteractableActor)
	AActor* CurrentInteractableActor;

	/** The interactable objects for the current interactable actor. */
	UPROPERTY(BlueprintGetter = GetCurrentInteractableObjects)
	TArray<FBHInteractableObjectData> CurrentInteractableObjects;

	/** The closest interactable object to the camera. */
	UPROPERTY()
	FBHInteractableObjectData ClosestInteractableObject;

	/** The actor that is currently being interacted with. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerInteraction", Meta = (DisplayName = "Current Interacting Actor", AllowPrivateAccess = "true"))
	AActor* CurrentInteractingActor;

	/** The collision query data for the camera trace. */
	struct FCollisionQueryParams CameraTraceQueryParams;

	/** The timer for the update function. */
	UPROPERTY()
	FTimerHandle UpdateTimerHandle;

#if WITH_EDITORONLY_DATA
	/** When true, we will draw debug visualisation to the screen for every collision query. */
	UPROPERTY(EditAnywhere, Category = "PlayerInteractionComponent|Debugging", Meta = (DisplayName = "Enable Debug Visualisation"))
	bool bDebugVisEnabled = false;
#endif
};
