#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "BHPlayerCharacterPhysicsHandleComponent.generated.h"

class ABHPlayerCharacter;
class ABHPlayerCharacterController;

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = "PlayerCharacter")
class STORMWATCH_API UBHPlayerCharacterPhysicsHandleComponent : public UPhysicsHandleComponent
{
	GENERATED_BODY()

public:
	UBHPlayerCharacterPhysicsHandleComponent();

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void OnControllerChanged(APawn* Pawn, AController* NewController, AController* PreviousController) {};

	class ABHPlayerCharacter* GetPlayerCharacter() const;
	class ABHPlayerCharacterController* GetPlayerCharacterController() const;
};
