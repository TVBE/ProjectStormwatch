// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerVfxComponent.h"
#include "BHPlayerCharacter.h"

#include "NiagaraComponent.h"

/** Sets default values for this component's properties. */
UBHPlayerVfxComponent::UBHPlayerVfxComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBHPlayerVfxComponent::OnRegister()
{
	Super::OnRegister();
	
	const ABHPlayerCharacter* PlayerCharacter = Cast<ABHPlayerCharacter>(GetOwner());
	if (!PlayerCharacter) { return ;}
	
	LeftFootEmitter = Cast<UNiagaraComponent>(GetOwner()->AddComponentByClass(UNiagaraComponent::StaticClass(), false, FTransform(), false));
	if (!LeftFootEmitter) { return; }
	LeftFootEmitter->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "foot_l_socket");
	LeftFootEmitter->bAutoActivate = false;
	LeftFootEmitter->bEditableWhenInherited = false;

	RightFootEmitter = Cast<UNiagaraComponent>(GetOwner()->AddComponentByClass(UNiagaraComponent::StaticClass(), false, FTransform(), false));
	if (!RightFootEmitter) { return; }
	RightFootEmitter->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "foot_r_socket");
	RightFootEmitter->bAutoActivate = false;
	RightFootEmitter->bEditableWhenInherited = false;
}



/** Called when the game starts. */
void UBHPlayerVfxComponent::BeginPlay()
{
	Super::BeginPlay();
	
}




/** Called every frame. */
void UBHPlayerVfxComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UBHPlayerVfxComponent::CleanupComponent()
{
	if (LeftFootEmitter)
	{
		LeftFootEmitter->Deactivate();
		LeftFootEmitter->DestroyComponent();
		LeftFootEmitter = nullptr;
	}
	if (RightFootEmitter)
	{
		RightFootEmitter->Deactivate();
		RightFootEmitter->DestroyComponent();
		RightFootEmitter = nullptr;
	}
}

void UBHPlayerVfxComponent::OnUnregister()
{
	CleanupComponent();
	Super::OnUnregister();
}

void UBHPlayerVfxComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CleanupComponent();
	Super::EndPlay(EndPlayReason);
}



