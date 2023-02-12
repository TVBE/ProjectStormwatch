// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerFlashlightController.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterMovementComponent.h"
#include "Components/SpotLightComponent.h"
#include "Frostbite/Frostbite.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values for this component's properties
UPlayerFlashlightController::UPlayerFlashlightController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
}


// Called when the game starts
void UPlayerFlashlightController::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
}


// Called every frame
void UPlayerFlashlightController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(PlayerCharacter && PlayerCharacter->GetCharacterMovement())
	{
		UpdateMovementAlpha(DeltaTime);
		//Frotator Rotation {}
	}
}


void UPlayerFlashlightController::UpdateMovementAlpha(const float DeltaTime)
{
	const bool IsMoving {PlayerCharacter->GetCharacterMovement()->IsMovingOnGround() && PlayerCharacter->GetCharacterMovement()->Velocity.Length() > 1};
	if(MovementAlpha != static_cast<int8>(IsMoving))
	{
			constexpr float InterpolationSpeed {6};
			MovementAlpha = FMath::FInterpTo(MovementAlpha, IsMoving, DeltaTime, InterpolationSpeed);
	}
}

FRotator UPlayerFlashlightController::GetFlashlightFocusRotation()
{
	const float Pitch {static_cast<float>(PlayerCharacter->GetControlRotation().Pitch)};
	if (UCameraComponent* Camera {PlayerCharacter->GetCamera()})
	{
		FVector Target {FVector()};
		
		FVector TraceStart {Camera->GetComponentLocation()};
		FVector TraceEnd {Camera->GetForwardVector() * 5000 + TraceStart};
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		if (this->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, CollisionParams))
		{
			Target = HitResult.ImpactPoint;
		}
		else
		{
			Target = TraceEnd;
		}

		constexpr float PitchRange {60};
		FRotator Rotation {FRotationMatrix::MakeFromX(TraceStart - Target).Rotator()};
		Rotation = FRotator(FMath::Clamp(Rotation.Pitch, -PitchRange, PitchRange), Rotation.Yaw, Rotation.Roll);
		return Rotation;
	}
	return FRotator();
}

FRotator UPlayerFlashlightController::GetFlashlightSwayRotation()
{
	FRotator Rotation {FRotator()};
	if(PlayerCharacter->GetPlayerCharacterMovement())
	{
		EPlayerGroundMovementType MovementType {PlayerCharacter->GetPlayerCharacterMovement()->GetGroundMovementType()};
		const float MappedVelocity {static_cast<float>(FMath::Clamp(PlayerCharacter->GetVelocity().Length() * 0.0325, 0.2f, 1.f))};
		constexpr float IntensityMultiplier {0.5};

		float PitchSwaySpeed {0.f};
		float YawSwaySpeed {0.f};
		float RollSwaySpeed {0.f};

		float PitchSwayIntensity {0.f};
		float YawSwayIntensity {0.f};
		float RollSwayIntensity {0.f};

		switch(MovementType)
		{
		case 0: PitchSwaySpeed = 1.65f;
			PitchSwayIntensity = 1.7f;
			YawSwaySpeed = 1.23f;
			YawSwayIntensity = 1.25f;
			RollSwaySpeed = 0.675f;
			RollSwayIntensity = 1.5f;
			break;
		case 1: PitchSwaySpeed = 3.12f * MappedVelocity;
			PitchSwayIntensity = 1.7f;
			YawSwaySpeed = 4.65 * MappedVelocity;
			YawSwayIntensity = 1.25f;
			RollSwaySpeed = 2.55f * MappedVelocity;
			RollSwayIntensity = 1.5f; ;
			break;
		case 2: PitchSwaySpeed = 9.55f;
			PitchSwayIntensity = 3.21f;
			YawSwaySpeed = 5.0f;
			YawSwayIntensity = 1.5f;
			RollSwaySpeed = 3.54f;
			RollSwayIntensity = 1.56f;
			break;
		}
		
		if(const UWorld* World {GetWorld()})
		{
			const double GameTime {World->GetTimeSeconds()};
			const float PitchIntensityMultiplier {static_cast<float>(FMath::GetMappedRangeValueClamped(FVector2D(-1, 1), FVector2D(0.75, 1.5),FMath::Cos(GameTime * 2.13f)))};
			const float YawIntensityMultiplier {static_cast<float>(FMath::GetMappedRangeValueClamped(FVector2D(-1, 1), FVector2D(0.75, 1.5),FMath::Cos(GameTime * 3.02f)))};
			PitchSwayIntensity = PitchSwayIntensity * PitchIntensityMultiplier;
			YawSwayIntensity = YawSwayIntensity * YawIntensityMultiplier;

			Rotation.Pitch = FMath::Cos(GameTime * PitchSwaySpeed) * PitchSwayIntensity * IntensityMultiplier;
			Rotation.Yaw = FMath::Cos(GameTime * YawSwaySpeed) * YawSwayIntensity * IntensityMultiplier;
			Rotation.Roll = FMath::Cos(GameTime * RollSwaySpeed) * RollSwayIntensity * IntensityMultiplier;
		}
		return Rotation;
	}
	return FRotator(); // TODO: Finish function implementation.
}

void UPlayerFlashlightController::SetFlashlightEnabled(bool Value)
{
	if(PlayerCharacter && PlayerCharacter->GetFlashlight() && PlayerCharacter->GetFlashlightSpringArm())
	{
		SetComponentTickEnabled(Value);
		PlayerCharacter->GetFlashlightSpringArm()->SetComponentTickEnabled(Value);
		PlayerCharacter->GetFlashlight()->SetVisibility(Value);
	}
}
	

bool UPlayerFlashlightController::IsFlashlightEnabled()
{
	if(PlayerCharacter && PlayerCharacter->GetFlashlight())
	{
		return PlayerCharacter->GetFlashlight()->IsVisible();
	}
	return false;
}



