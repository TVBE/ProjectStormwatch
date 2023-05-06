
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerDragComponent.generated.h"

class UCameraComponent;
class APlayerCharacter;



UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Within = "PlayerCharacter",
	Meta = (DisplayName = "Player Drag Component", ShortToolTip = "Component for dragging physics objects."))
class FROSTBITE_API UPlayerDragComponent : public UPhysicsHandleComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogDragComponent, Log, All)

public:


protected:
	void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	/** Returns the actor that is currently being grabbed. */
	UFUNCTION(BlueprintCallable, Category = "Player Physics Grab", Meta = (DisplayName = "Get Current Grabbed Actor"))
	FORCEINLINE AActor* GetDraggedActor() const
	{
		if (const UPrimitiveComponent* Component {GetGrabbedComponent()}) { return Component->GetOwner(); }
		return nullptr; 
	}

	UPROPERTY(EditDefaultsOnly, Category = "Drag Configuration")
	UDragConfiguration* Configuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY()
	float CameraRotationMultiplier{1.0f};

	UFUNCTION(BlueprintCallable, Category = "Drag")
	void DragActorAtLocation(AActor* ActorToDrag, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Drag")
	void ReleaseActor();

	UFUNCTION(BlueprintCallable, Category = "Zoom")
	void UpdateZoomAxisValue(float ZoomAxis);

private:

	void UpdateTargetLocation(float DeltaTime);

	void UpdateCameraRotationSpeed(float DeltaTime);

	
	UFUNCTION()
	void ApplyToPhysicsHandle();
	
	bool ApplyForceOnCenterMass{false};
	
	UPROPERTY()
	class UPlayerCharacterMovementComponent* Movement;

	float Gravity;

	FVector DraggedLocationOffset;


	float DraggedComponentSize;

	FVector TargetLocation;


	float CurrentZoomLevel;


	float CurrentZoomAxisValue;

};
/** Configuration asset to fine tune all variables within the drag component*/
UCLASS(BlueprintType, ClassGroup = "PlayerCharacter")
class FROSTBITE_API UDragConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab Settings")
	float LetGoDistance{300.f};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom Settings")
	float ZoomSpeed{0.0f};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom Settings")
	float MinZoomLevel{0.0f};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom Settings")
	float MaxZoomLevel{1000.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Player Physics Grab")
	float CameraRotationDecreasingStrength{1.0f};
	
	
	// ... PhysicsHandleSettings ... 

	/** Linear damping of the handle spring. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PhysicsHandle", Meta = (EditCondition = "bSoftLinearConstraint"))
	float LinearDamping{1000.0f};

	/** Linear stiffness of the handle spring */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PhysicsHandle", Meta = (EditCondition = "bSoftLinearConstraint"))
	float LinearStiffness{1000.0f};

	/** Angular damping of the handle spring */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PhysicsHandle", Meta = (EditCondition = "bSoftAngularConstraint"))
	float AngularDamping{0.0f};

	/** Angular stiffness of the handle spring */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PhysicsHandle", Meta = (EditCondition = "bSoftAngularConstraint"))
	float AngularStiffness{0.0f};

	/** How quickly we interpolate the physics target transform */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PhysicsHandle", Meta = (EditCondition = "bInterpolateTarget"))
	float InterpolationSpeed{1.0f};
};
