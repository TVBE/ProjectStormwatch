// Copyright Notice

#pragma once

#include "CoreMinimal.h"
#include "UsableObjectInterface.h"
#include "Components/SceneComponent.h"
#include "MeshUseComponent.generated.h"


UCLASS(NotBlueprintable, BlueprintType, ClassGroup = "Interaction", Meta = (BlueprintSpawnableComponent,
	DisplayName = "Mesh Use Component", ShortToolTip = "Component that allows the player to use an actor."))
class FROSTBITE_API UMeshUseComponent : public USceneComponent, public IUsableObject
{
	GENERATED_BODY()

public:	
	UMeshUseComponent();

protected:

	virtual void BeginPlay() override;

public:	
	
};
