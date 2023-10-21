// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BHNightstalker.generated.h"

class UBHNightstalkerMovementComponent;
class UInputComponent;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Nightstalker")
class STORMWATCH_API ABHNightstalker : public ACharacter
{
	GENERATED_BODY()
	
	DECLARE_LOG_CATEGORY_CLASS(LogNightstalker, Log, All)

private:
	UPROPERTY(BlueprintGetter = GetNightstalkerMovementComponent)
	UBHNightstalkerMovementComponent* NightstalkerMovementComponent;
	
	UPROPERTY(BlueprintGetter = GetIsPhantomModeEnabled)
	bool IsPhantomModeEnabled = false;

public:
	ABHNightstalker();

	virtual void PostInitProperties() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Enables or disables phantom mode. In phantom mode, the Nightstalker is invisible, but will still produce all sounds as normal.
	 *  This is useful to let the Nightstalker traverse terrain from a distance without needing to worry about visual occlusion. */
	UFUNCTION(BlueprintCallable, Category = "Phantom Mode")
	void SetPhantomModeEnabled(const bool Value);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "Update Phantom Mode"))
	void EventSetPhantomModeEnabled(const bool Value);

public:
	UFUNCTION(BlueprintGetter, Meta = (DisplayName = "Movement Component"))
	FORCEINLINE UBHNightstalkerMovementComponent* GetNightstalkerMovementComponent() const { return NightstalkerMovementComponent;}
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsPhantomModeEnabled() const { return IsPhantomModeEnabled; }
};

USTRUCT(BlueprintType)
struct FNightstalkerMovementAttributes
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxWalkSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Acceleration = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationSpeed = 180.0f;
};


