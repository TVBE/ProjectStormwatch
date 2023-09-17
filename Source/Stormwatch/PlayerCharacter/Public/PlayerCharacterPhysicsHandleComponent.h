#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PlayerCharacterPhysicsHandleComponent.generated.h"

class APlayerCharacter;
class APlayerCharacterController;

UCLASS(Abstract)
class STORMWATCH_API UPlayerCharacterPhysicsHandleComponent : public UPhysicsHandleComponent
{
	GENERATED_BODY()

public:
	UPlayerCharacterPhysicsHandleComponent();

	virtual bool IsValid() const { return true; }

protected:
	virtual void OnRegister() override;


	APlayerCharacter* GetPlayerCharacter() const;

	APlayerCharacterController* GetPlayerCharacterController() const;
};
