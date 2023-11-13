// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPlayerCharacterComponent.h"
#include "BHPlayerVfxComponent.generated.h"

class UNiagaraComponent;
class ABHPlayerCharacter;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "BHPlayerCharacter")
class STORMWATCH_API UBHPlayerVfxComponent : public UBHPlayerCharacterComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintGetter = GetLeftFootParticleEmitter)
	UNiagaraComponent* LeftFootEmitter;
	
	UPROPERTY(BlueprintGetter = GetRightFootParticleEmitter)
	UNiagaraComponent* RightFootEmitter;
	
public:	
	UBHPlayerVfxComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void OnRegister() override;

	virtual void OnUnregister() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void CleanupComponent();

public:
	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Left Foot Particle Emitter"))
	UNiagaraComponent* GetLeftFootParticleEmitter() const {return LeftFootEmitter; }

	UFUNCTION(BlueprintGetter, Category = "PlayerCharacter|Components", Meta = (DisplayName = "Right Foot Particle Emitter"))
	UNiagaraComponent* GetRightFootParticleEmitter() const {return RightFootEmitter; }
};
