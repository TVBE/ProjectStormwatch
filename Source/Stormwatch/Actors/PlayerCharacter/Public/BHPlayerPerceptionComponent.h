// Copyright Notice

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "BHPlayerPerceptionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNightstalkerPerceptionChangedDelegate, bool, bNightstalkerDetected);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "BHPlayerCharacter", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHPlayerPerceptionComponent : public UBHPlayerCharacterComponent
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
	float PerceptionUpdateInterval = 0.25f;

protected:
	UPROPERTY()
	class UCameraComponent* Camera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float NightstalkerDetectionThreshold = 0.3f;

	UPROPERTY(BlueprintReadWrite)
	bool bNightstalkerVisible = false;

	bool bNightstalkerDetected = false;

	UPROPERTY(BlueprintReadOnly)
	float NightstalkerVisibilityDuration = 0.0f;

public:
	UBHPlayerPerceptionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void OnRegister() override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, Category = "Events", Meta = (DisplayName = "On Perception Update"))
	void UpdatePerception();

	UFUNCTION(BlueprintCallable)
	float GetViewAngleToTarget(const FVector& Target);
};
