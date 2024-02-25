// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BHPlayerCharacterComponent.h"
#include "Components/ActorComponent.h"
#include "BHPlayerAttributeComponent.generated.h"

UENUM(BlueprintType)
enum class EBHAttributeRegenerationMethod : uint8
{
	Constant,
	Curve
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMinValueReachedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMaxValueReachedSignature);

UCLASS(Blueprintable, BlueprintType, ClassGroup = "BHPlayerCharacter", Meta = (BlueprintSpawnableComponent))
class STORMWATCH_API UBHPlayerAttributeComponent : public UActorComponent, public FBHPlayerCharacterComponent
{
	GENERATED_BODY()

public:
	UBHPlayerAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "BHPlayerAttributeComponent")
	void DecrementValue(float Amount, bool bClamp = false, float Floor = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "BHPlayerAttributeComponent")
	void Incrementvalue(float Amount, bool bClamp = false, float Ceiling = 0.0f);

	UFUNCTION(BlueprintGetter)
	float GetValue() const;

	UFUNCTION(BlueprintCallable, Category = "BHPlayerAttributeComponent")
	void SetValue(float NewValue);
	
	UFUNCTION(BlueprintGetter)
	float GetMaxValue() const;
	
	UFUNCTION(BlueprintCallable, Category = "BHPlayerAttributeComponent")
	void SetMaxValue(float NewValue);

	UFUNCTION(BlueprintGetter)
	bool IsRegenerationEnabled() const;

	UFUNCTION(BlueprintCallable, Category = "BHPlayerAttributeComponent")
	void SetRegenerationEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "BHPlayerAttributeComponent")
	void ResetRegeneration();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnValueChangedSignature OnValueChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMinValueReachedSignature OnMinValueReached;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMaxValueReachedSignature OnMaxValueReached;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintGetter = GetValue, Category = "Attribute")
	float Value = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintGetter = GetMaxValue, Category = "Attribute")
	float MaxValue = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintGetter = IsRegenerationEnabled, Category = "Attribute|Regeneration")
	bool bEnableRegeneration = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute|Regeneration",
	Meta = (EditCondition = "bEnableRegeneration", Units = "Seconds"))
	float RegenerationDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute|Regeneration")
	EBHAttributeRegenerationMethod RegenerationMethod = EBHAttributeRegenerationMethod::Constant;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute|Regeneration")
	float RegenerationRate = 5.0f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute|Regeneration",
	Meta = (EditCondition = "bEnableRegeneration", XAxisName = "Time", YAxisName = "Rate"))
	FRuntimeFloatCurve RegenerationRateCurve;

private:
	float TimeSinceLastDecrement = 0.0f;
};
