// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne & Nino Saglia
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInteractionComponent.generated.h"

class UPlayerPhysicsGrabConfiguration;
class UPlayerDragComponent;
class UPlayerUseComponent;
class UPlayerInventoryComponent;
class UPlayerGrabComponent;
class UCameraComponent;
struct FCollisionQueryParams;

/** The interaction action type. */
UENUM(BlueprintType)
enum class EInteractionActionType : uint8
{
	Primary					UMETA(DisplayName = "Primary", ToolTip = "The primary action is used to 'use' actors."),
	Secondary				UMETA(DisplayName = "Secondary", ToolTip = "The secondary action is used to grab actors."),
	Inventory				UMETA(DisplayName = "Inventory", ToolTip = "The inventory action is used to add or take objects from the inventory.")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableActorFoundDelegate, AActor*, InteractableActor);

/** PlayerCharacter component that checks for interactable objects in front of the player character. */
UCLASS(Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter",
	Meta = (DisplayName = "Player Interaction Component", ShortToolTip = "Component that handles player interaction."))
class FROSTBITE_API UPlayerInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Delegate called when the CurrentInteractableActor property is changed. */
	UPROPERTY(BlueprintAssignable, Category = "PlayerInteractionComponent|Delegates", Meta = (DisplayName = "On Interactable Actor Found"))
	FInteractableActorFoundDelegate OnInteractableActorFound;

	/** The length of the initial line trace. */
	UPROPERTY(EditDefaultsOnly, Category = "PlayerInteraction", Meta = (DisplayName = "Camera Trace Length", ClampMax = "500", UIMax = "500"))
	uint16 CameraTraceLength {300};

	/** The radius of the interactable object multi sphere trace. */
	UPROPERTY(EditDefaultsOnly, Category = "PlayerInteraction", Meta = (DisplayName = "Object Trace Radius", ClampMax = "500", UIMax = "500"))
	uint16 ObjectTraceRadius {50};

private:
	/** The use component that is used to use actors. */
	UPROPERTY(BlueprintGetter = GetUseComponent, Category = "PlayerInteraction|Components", Meta = (DisplayName = "Use Component"))
	UPlayerUseComponent* UseComponent;
	
	/** The physics grab component that is used to grab actors. */
	UPROPERTY(BlueprintGetter = GetGrabComponent, Category = "PlayerInteraction|Components", Meta = (DisplayName = "Grab Component"))
	UPlayerGrabComponent* GrabComponent;

	/** The physics drag component that is used to drag actors. */
	UPROPERTY(BlueprintGetter = GetDragComponent, Category = "PlayerInteraction|Components", Meta = (DisplayName = "Drag Component"))
	UPlayerDragComponent* DragComponent;

	/** The inventory component that is used to store actors in the player's inventory.
	 *	The inventory component is not added by this component.
	 *	Therefore, we cannot always assume that the component is actually present in the playercharacter instance. */
	UPROPERTY()
	UPlayerInventoryComponent* InventoryComponent;
	
	/** The camera component of the Player Character. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerInteraction|Components", Meta = (DisplayName = "Camera", AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** If true, the interaction component is currently performing a tertiary interaction. */
	UPROPERTY(BlueprintGetter = GetIsTertiaryInteractionActive, Category = "PlayerInteraction|Components", Meta = (DisplayName = "Is Performing Tertiary Interaction", AllowPrivateAccess = "true"))
	bool IsTertiaryInteractionActive {false};
	
	/** The location of the camera. */
	UPROPERTY()
	FVector CameraLocation;
	
	/** The hit result for the initial visibility line trace collision query performed from the camera. */
	UPROPERTY()
	FHitResult CameraTraceHitResult {FHitResult()};

	/** The hit result for the occlusion line trace collision query performed from the camera. */
	UPROPERTY()
	FHitResult OcclusionTraceHitResult {FHitResult()};

	/** The offset used for the occlusion trace. Prevents the occlusion trace hitting the actor underneath
	 *	the interactable object that the occlusion trace is performed for. */
	UPROPERTY()
	FVector OcclusionOffset {FVector(0, 0, 5)};

	/** The array of hit results for the interactable object multi sphere trace. */
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TArray<FHitResult> ObjectTraceHitResults;

	/** The actor that currently can be interacted with. Will be a nullptr if no object can be interacted with at the moment. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerInteraction", Meta = (DisplayName = "Current Interactable Actor", AllowPrivateAccess = "true"))
	AActor* CurrentInteractableActor;

	/** The actor that could be interacted with during the previous frame. */
	UPROPERTY()
	AActor* PreviousInteractableActor;

	/** The actor that is currently being interacted with. */
	UPROPERTY(BlueprintReadOnly, Category = "PlayerInteraction", Meta = (DisplayName = "Current Interacting Actor", AllowPrivateAccess = "true"))
	AActor* CurrentInteractingActor;
	
	/** The collision query data for the camera trace. */
	FCollisionQueryParams CameraTraceQueryParams;
	
	/** The timer for the update function. */
	UPROPERTY()
	FTimerHandle UpdateTimerHandle;
	
	/** For setting the parameters of the playerPhysicsGrabComponent. */
	UPROPERTY(EditAnywhere, Category = "PlayerInteractionComponent", Meta = (DisplayName = "Settings for player physics grab compoent"))
	TSoftObjectPtr<UPlayerPhysicsGrabConfiguration> PlayerPhysicsGrabConfiguration;

#if WITH_EDITORONLY_DATA
	/** When true, we will draw debug visualisation to the screen for every collision query. */
	UPROPERTY(EditAnywhere, Category = "PlayerInteractionComponent|Debugging", Meta = (DisplayName = "Enable Debug Visualisation"))
	bool IsDebugVisEnabled {false};
#endif

public:	
	UPlayerInteractionComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Begins the primary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteration|Actions", Meta = (DisplayName = "Begin Primary Interaction", BlueprintProtected))
	void BeginPrimaryInteraction();
	
	/** Ends the primary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteraction|Actions", Meta = (DisplayName = "End Primary Interaction", BlueprintProtected))
	void EndPrimaryInteraction();
	
	/** Begins the secondary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteraction|Actions", Meta = (DisplayName = "Begin Secondary Interaction", BlueprintProtected))
	void BeginSecondaryInteraction();

	/** Ends the secondary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteraction|Actions", Meta = (DisplayName = "End Secondary Interaction", BlueprintProtected))
	void EndSecondaryInteraction();

	/** Begins the tertiary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteraction|Actions", Meta = (DisplayName = "Begin Tertiary Interaction", BlueprintProtected))
	void BeginTertiaryInteraction();

	/** Ends the tertiary interaction action. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteraction|Actions", Meta = (DisplayName = "End Tertiary Interaction", BlueprintProtected))
	void EndTertiaryInteraction();

	/** Begins the inventory action. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteraction|Actions", Meta = (DisplayName = "Begin Inventory Interaction", BlueprintProtected))
	void BeginInventoryInteraction();

	/** Ends the inventory action. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteraction|Actions", Meta = (DisplayName = "End Inventory Interaction", BlueprintProtected))
	void EndInventoryInteraction();
	
	/** Handles scroll input. */
	UFUNCTION(BlueprintCallable, Category = "PlayerInteraction|Actions", Meta = (DisplayName = "Add Scroll Input", BlueprintProtected))
	void AddScrollInput(const float Input);

	/** Adds pitch input to the interaction component. */
	UFUNCTION()
	void AddPitchInput(const float Input);

	/** Adds yaw input to the interaction component. */
	UFUNCTION()
	void AddYawInput(const float Input);
	
	/** Checks if an actor or one of its components implements the IInteractableObject interface.
	*	Returns the first UObject that implements the interface that it finds. */
	template <typename TInterface>
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
	template <typename TInterface>
	UActorComponent* FindInteractableComponent(const AActor* Actor) const;

	/** Converts a UObject pointer to an AActor pointer.
	 *	Either by checking if the UObject is an AActor,
	 *	or by casting the UObject to an UActorComponent and retrieving the owner from the component. */
	UFUNCTION()
	AActor* GetActorFromObject(UObject* Object) const;

public:
	/** Returns a pointer to the use component. */
	UFUNCTION(BlueprintGetter, Category = "PlayerInteraction|Components", Meta = (DisplayName = "Use Component"))
	FORCEINLINE UPlayerUseComponent* GetUseComponent() const { return UseComponent; }

	/** Returns a pointer to the grab component. */
	UFUNCTION(BlueprintGetter, Category = "PlayerInteraction|Components", Meta = (DisplayName = "Grab Component"))
	FORCEINLINE UPlayerGrabComponent* GetGrabComponent() const { return GrabComponent; }

	/** Returns a pointer to the drag component. */
	UFUNCTION(BlueprintGetter, Category = "PlayerInteraction|Components", Meta = (DisplayName = "Drag Component"))
	FORCEINLINE UPlayerDragComponent* GetDragComponent() const { return DragComponent; }
	
	/** Returns the object the interaction component is currently interacting with. */
	UFUNCTION(BlueprintPure, Category = "PlayerInteraction", Meta = (DisplayName = "Current Interaction Object"))
	FORCEINLINE AActor* GetCurrentInteractingActor() const { return CurrentInteractingActor; }

	/** Returns whether the interaction component is currently performing a tertiary interaction or not. */
	UFUNCTION(BlueprintGetter, Category = "PlayerInteraction", Meta = (DisplayName = "Is Tertiary Interaction Active"))
	FORCEINLINE bool GetIsTertiaryInteractionActive() const { return IsTertiaryInteractionActive; }

	/** Returns whether there is an object in front of the player that can be interacted with. */
	UFUNCTION(BlueprintPure, Category = "PlayerInteraction", Meta = (DisplayName = "Can Interact"))
	FORCEINLINE bool CanInteract() const { return static_cast<bool>(CurrentInteractableActor); }

	/** Returns a pointer to an object that is currently available for interaction. Returns a nullptr if no interactable object is in front of the player. */
	UFUNCTION(BlueprintPure, Category = "PlayerInteraction", Meta = (DisplayName = "Get Current Interactable Actor"))
	FORCEINLINE AActor* GetCurrentInteractableActor() const { return CurrentInteractableActor; }

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "PlayerInteraction", Meta = (DisplayName = "Begin Interaction"))
	void EventBeginInteraction(const EInteractionActionType Type, const UObject* Object);
    
	UFUNCTION(BlueprintNativeEvent, Category = "PlayerInteraction", Meta = (DisplayName = "End Interaction"))
	void EventEndInteraction(const EInteractionActionType Type, const UObject* Object);
};

