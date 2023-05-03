
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerDragComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FROSTBITE_API UPlayerDragComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogDragComponent, Log, All)

public:
	UPlayerDragComponent();
	void OnRegister();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = "Drag Configuration")
	UDragConfiguration* Configuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* Camera;

	UFUNCTION(BlueprintCallable, Category = "Drag")
	void DragActorAtLocation(AActor* ActorToDrag, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "Drag")
	void ReleaseActor();

	UFUNCTION(BlueprintCallable, Category = "Zoom")
	void UpdateZoomAxisValue(float ZoomAxis);

private:
	void UpdateTargetForceWithLocation(float DeltaTime);
	
	void ApplyTargetForce(FVector TargetLocation, bool ApplyForceOnCenterMass);


	void UpdateTargetLocationWithRotation(float DeltaTime);

	void DragComponentAtLocationWithRotation(class UPrimitiveComponent* InComponent, FName InSocketName, FVector InLocation, FRotator InRotation);
	
	
	bool ApplyForceOnCenterMass{false};

	UPROPERTY()
	UPrimitiveComponent* DraggedComponent;

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
	float LetGoDistance{50000.f};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom Settings")
	float ZoomSpeed{500.f};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom Settings")
	float MinZoomLevel{100.f};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom Settings")
	float MaxZoomLevel{1000.f};


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Force Settings")
	float ApplyTargetForce{1000.f};
};