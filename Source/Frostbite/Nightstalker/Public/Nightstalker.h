// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Nightstalker.generated.h"

class UNightstalkerMovementComponent;
class UInputComponent;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "Nightstalker")
class ANightstalker : public ACharacter
{
	GENERATED_BODY()
	
	DECLARE_LOG_CATEGORY_CLASS(LogNightstalker, Log, All)

private:
	UPROPERTY(BlueprintGetter = GetNightstalkerMovementComponent)
	UNightstalkerMovementComponent* NightstalkerMovementComponent;
	
	UPROPERTY(BlueprintGetter = GetIsPhantomModeEnabled)
	bool IsPhantomModeEnabled {false};

public:
	ANightstalker();

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
	FORCEINLINE UNightstalkerMovementComponent* GetNightstalkerMovementComponent() const { return NightstalkerMovementComponent;}
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsPhantomModeEnabled() const { return IsPhantomModeEnabled; }
};

UENUM(BlueprintType)
enum class ENightstalkerBehavior : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Roaming		    UMETA(DisplayName = "Roaming"),
};


