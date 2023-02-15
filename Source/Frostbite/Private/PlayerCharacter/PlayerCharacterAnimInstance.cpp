// Copyright 2023 Barrelhouse


#include "PlayerCharacterAnimInstance.h"

FFootstepData UPlayerCharacterAnimInstance::GetFootstepData(EFoot Foot)
{
	FFootstepData FootstepData {FFootstepData()};
	FootstepData.Foot = Foot;
	const FName Socket {Foot == EFoot::Left ? "foot_l_Socket" : "foot_r_Socket"};
	if(GetSkelMeshComponent() && GetSkelMeshComponent()->GetOwner())
	{
		FVector Location {GetSkelMeshComponent()->GetSocketLocation(Socket)};
		FootstepData.Location = Location;
		FootstepData.Velocity = GetSkelMeshComponent()->ComponentVelocity.Length();
		FHitResult HitResult;
		FVector TraceStart = Location;
		FVector TraceEnd = Location + FVector(0, 0, -30);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetSkelMeshComponent()->GetOwner());
		Params.bTraceComplex = false;

		if (GetWorld() && GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params))
		{
			FootstepData.Object = HitResult.GetActor();
			FootstepData.PhysicalMaterial = HitResult.PhysMaterial.Get();
		}
	}
	return FootstepData;
}
