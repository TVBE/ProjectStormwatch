// Copyright 2023 Barrelhouse

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NightstalkerBehaviorScript.generated.h"

class ANightstalker;
class ANightstalkerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivationDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeactivationDelegate);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (Nightstalker))
class UNightstalkerBehaviorScript : public UActorComponent
{
	GENERATED_BODY()

public:
	/** The delegate that is called when the script is activated. */
	UPROPERTY(BlueprintAssignable, Category = "BehaviorScrip|Delegates", Meta = (DisplayName = "On Script Activation"))
	FOnActivationDelegate OnActivation;

	/** The delegate that is called when the script is deactivated. */
	UPROPERTY(BlueprintAssignable, Category = "BehaviorScrip|Delegates", Meta = (DisplayName = "On Script Deactivation"))
	FOnDeactivationDelegate OnDeactivation;
	
private:
	/** Pointer to the nightstalker instance that this behavior script is controlling. */
	UPROPERTY(BlueprintGetter = GetNightstalker, Category = "BehaviorScript", Meta = (DisplayName = "Nightstalker"))
	ANightstalker* Nightstalker;

	/** If true, the behavior script is currently active. */
	UPROPERTY()
	bool IsScriptActive {false};
	
public:
	UNightstalkerBehaviorScript();
	
	/** Activates the behavior script.
	 *	Param Controller The controller that 'owns' this behavior script.
	 */
	UFUNCTION(BlueprintCallable, Category = "BehaviorScript", Meta = (DisplayName = "Activate Script"))
	void ActivateScript();

	/** Deactivates the behavior script.
	 *	Param Controller The controller that 'owns' this behavior script.
	 */
	UFUNCTION(BlueprintCallable, Category = "BehaviorScript", Meta = (DisplayName = "Deactivate Script"))
	void DeactivateScript();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/** Returns a pointer to the nightstalker instance that this behavior script is controlling. */
	UFUNCTION(BlueprintGetter, Category = "BehaviorScript", Meta = (DisplayName = "Nightstalker", BlueprintProtected))
	FORCEINLINE ANightstalker* GetNightstalker() const {return Nightstalker; }
};
