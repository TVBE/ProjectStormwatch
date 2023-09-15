// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerVfxComponent.generated.h"

class UNiagaraComponent;
class APlayerCharacter;

/** UPlayerAudioController is an Actor Component responsible for managing all VFX specific to the player character. 
 *	This class provides a simple and convenient way for designers to customize the player's VFX implementation.
 *	@Brief ActorComponent for managing player visual effects.
 */
UCLASS(Blueprintable, ClassGroup = "PlayerCharacter", Meta = (BlueprintSpawnableComponent,
	DisplayName = "Player VFX Component", ShortToolTip = "Component that manages player VFX."))
class STORMWATCH_API UPlayerVfxComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintGetter = GetLeftFootParticleEmitter)
	UNiagaraComponent* LeftFootEmitter;
	
	UPROPERTY(BlueprintGetter = GetRightFootParticleEmitter)
	UNiagaraComponent* RightFootEmitter;
	
public:	
	UPlayerVfxComponent();

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
