// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHInteractionComponent.h"

void UBHInteractionComponent::OnRegister()
{
	Super::OnRegister();
	
	if (bAllowOnlyOneInstancePerActor)
	{
		if (AActor* Owner = GetOwner())
		{
			const TSet<UActorComponent*>& Components = Owner->GetComponents();

			const auto DuplicateComponent = Algo::Find(Components, [this](UActorComponent* Component) 
			{
				return Component != this && Component->GetClass() == this->GetClass();
			});
			
			ensureAlwaysMsgf(!DuplicateComponent, TEXT("Cannot add multiple instances of %s to a single actor."),
				*this->GetClass()->GetName());
		}
	}
}
