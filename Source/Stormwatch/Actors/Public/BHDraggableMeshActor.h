// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#pragma once

#include "BHPhysicsMeshActor.h"
#include "BHDraggableMeshActor.generated.h"

// A mesh actor that can be dragged by the player.
UCLASS(Blueprintable, BlueprintType, ClassGroup = "Barrelhouse")
class STORMWATCH_API ABHDraggableMeshActor : public ABHPhysicsMeshActor
{
	GENERATED_BODY()

public:
	ABHDraggableMeshActor();

	UFUNCTION(BlueprintGetter)
	class UBHDragComponent* GetDragComponent() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintGetter = GetDragComponent, Category = "Components")
	UBHDragComponent* DragComponent;
};
