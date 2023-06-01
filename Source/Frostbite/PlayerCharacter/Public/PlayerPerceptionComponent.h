// Copyright Notice

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerPerceptionComponent.generated.h"

class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNightstalkerPerceptionChangedDelegate, bool, IsNightstalkerDetected);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter", Meta = (BlueprintSpawnableComponent))
class FROSTBITE_API UPlayerPerceptionComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_LOG_CATEGORY_CLASS(LogPlayerPerception, Log, All)

public:
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnNightstalkerPerceptionChangedDelegate OnNightstalkerPerceptionChanged;

private:
	UPROPERTY()
	FTimerHandle PerceptionTimerHandle;

	UPROPERTY(EditAnywhere)
	float PerceptionUpdateInterval {0.25f};

protected:
	UPROPERTY()
	UCameraComponent* Camera {nullptr};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float NightstalkerDetectionThreshold {300.0f};

	UPROPERTY(BlueprintReadWrite)
	bool IsNightstalkerVisible {false};

	bool IsNightstalkerDetected {false};

	UPROPERTY(BlueprintReadOnly)
	float NightstalkerVisibilityDuration {0.0f};

public:
	UPlayerPerceptionComponent();

protected:
	virtual void OnRegister() override;
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "On Perception Update"))
	void UpdatePerception();

	UFUNCTION(BlueprintCallable)
	float GetViewAngleToTarget(const FVector& Target);
};
