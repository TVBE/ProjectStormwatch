// Copyright 2023 Barrelhouse


#include "PlayerCharacterAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "PlayerCharacterMovementComponent.h"


void UPlayerCharacterAnimInstance::NativeInitializeAnimation()
{
	if(GetSkelMeshComponent() && GetSkelMeshComponent()->GetOwner())
	{
		PlayerCharacter = Cast<APlayerCharacter>(GetSkelMeshComponent()->GetOwner());
	}
	Super::NativeInitializeAnimation();
}

void UPlayerCharacterAnimInstance::NativeBeginPlay()
{
	if(!PlayerCharacter && GetSkelMeshComponent() && GetSkelMeshComponent()->GetOwner())
	{
		PlayerCharacter = Cast<APlayerCharacter>(GetSkelMeshComponent()->GetOwner());
	}
	Super::NativeBeginPlay();
}

void UPlayerCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(PlayerCharacter)
	{
		const APlayerCharacterController* Controller {PlayerCharacter->GetPlayerCharacterController()};
		const UPlayerCharacterMovementComponent* CharacterMovement {PlayerCharacter->GetPlayerCharacterMovement()};
		if(Controller && CharacterMovement)
		{
			CheckMovementState(*PlayerCharacter, *Controller, *CharacterMovement);
		}

		Direction = GetDirection(*PlayerCharacter);
		Speed = GetSpeed(*PlayerCharacter, *CharacterMovement);

		// Reset fall timer if the player is no longer falling.
		if(IsFalling ^ CharacterMovement->IsFalling() && !IsFalling)
		{
			FallTime = 0.0f;
		}

		// Check whether the player is falling.
		IsFalling = CharacterMovement->IsFalling();
		IsAirborne = CharacterMovement->IsFalling() || CharacterMovement->GetIsJumping();

		// Update fall time if the player is falling.
		if(IsFalling)
		{
			UpdateFallTime(DeltaSeconds);
		}
		
		CheckTurnInplaceConditions(*PlayerCharacter);
	}
	Super::NativeUpdateAnimation(DeltaSeconds);
}

FFootstepData UPlayerCharacterAnimInstance::GetFootstepData(EFoot Foot)
{
	FFootstepData FootstepData {FFootstepData()};
	FootstepData.Foot = Foot;
	const FName Socket {Foot == EFoot::Left ? "foot_l_Socket" : "foot_r_Socket"};
	if(GetSkelMeshComponent() && GetSkelMeshComponent()->GetOwner())
	{
		FVector Location {GetSkelMeshComponent()->GetSocketLocation(Socket)};
		FootstepData.Location = Location;

		if(GetSkelMeshComponent() && GetSkelMeshComponent()->GetOwner())
		{
			FootstepData.Velocity = GetSkelMeshComponent()->GetOwner()->GetVelocity().Length();
		}
		
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

void UPlayerCharacterAnimInstance::CheckMovementState(const APlayerCharacter& Character, const APlayerCharacterController& Controller, const UPlayerCharacterMovementComponent& CharacterMovement)
{
	IsMovementPending = Controller.GetHasMovementInput();
	IsMoving = IsMovementPending && (CharacterMovement.IsMovingOnGround() || CharacterMovement.IsFalling());

	DoSprintSop = !IsMovementPending && Speed > 275 && (Direction >= -20 && Direction <= 20);
}

void UPlayerCharacterAnimInstance::CheckTurnInplaceConditions(const APlayerCharacter& Character)
{
	if(Character.GetIsTurningInPlace())
	{
		if(Character.GetYawDelta() > 0)
		{
			IsTurningRight = true;
			IsTurningLeft = false;
		}
		else
		{
			IsTurningRight = false;
			IsTurningLeft = true;
		}
		TurnSpeed = FMath::Clamp(0.1f * abs(Character.GetYawDelta()), 0.0f, 1.0f);
	}
	else
	{
		IsTurningLeft = false;
		IsTurningRight = false;
		TurnSpeed = 0.0f;
	}
}

float UPlayerCharacterAnimInstance::GetDirection(const APlayerCharacter& Character)
{
	const float UnmappedDirection {UKismetAnimationLibrary::CalculateDirection(Character.GetVelocity(), Character.GetActorRotation())};
	return FMath::GetMappedRangeValueClamped(FVector2D(-171.5, 171.5), FVector2D(-180, 180), UnmappedDirection);
}

float UPlayerCharacterAnimInstance::GetSpeed(const APlayerCharacter& Character, const UPlayerCharacterMovementComponent& CharacterMovement)
{
	if(!CharacterMovement.GetLastInputVector().IsNearlyZero())
	{
		return Character.GetVelocity().Size2D();
	}
	return 0.0f;
}

void UPlayerCharacterAnimInstance::UpdateFallTime(const float& DeltaTime)
{
	FallTime += DeltaTime;
	VerticalAlpha = FMath::Clamp(FallTime, 0, 1);
}
