// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerController.h"

#include "BHPlayerCameraSocketComponent.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerDragComponent.h"
#include "BHPlayerGrabComponent.h"
#include "BHPlayerInteractionComponent.h"
#include "..\..\..\Core\Public\BHStormwatchWorldSubystem.h"

#include "Math/Rotator.h"
#include "Camera/CameraComponent.h"

ABHPlayerController::ABHPlayerController()
{
	InteractionComponent = CreateDefaultSubobject<UBHPlayerInteractionComponent>(TEXT("InteractionComponent"));
	if (InteractionComponent)
	{
		InteractionComponent->PrimaryComponentTick.bCanEverTick = true;
		InteractionComponent->PrimaryComponentTick.bStartWithTickEnabled = false;
		InteractionComponent->PrimaryComponentTick.TickGroup = TG_PostPhysics;
	}
}

void ABHPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ABHPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn) { return; }

	PlayerCharacter = Cast<ABHPlayerCharacter>(this->GetPawn());

	if (!PlayerCharacter) { return;  }
	
	/** Registers the controller to the player character subsystem. */
	if (const UWorld* World = GetWorld())
	{
		if (UBHStormwatchWorldSubsystem* PlayerSubsystem = World->GetSubsystem<UBHStormwatchWorldSubsystem>())
		{
			PlayerSubsystem->Register(this);
		}
	}

	if (const UBHPlayerCameraSocketComponent* CameraController = PlayerCharacter->GetCameraController()) 
	{
		PlayerCameraManager->ViewPitchMax = CameraController.MaximumViewPitch;
		PlayerCameraManager->ViewPitchMin = CameraController.MinimumViewPitch;
	}
}

void ABHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent ();

	InputComponent->BindAxis(TEXT("Horizontal Rotation"), this, &ABHPlayerController::HandleHorizontalRotation);
	InputComponent->BindAxis(TEXT("Vertical Rotation"), this,  &ABHPlayerController::HandleVerticalRotation);
	InputComponent->BindAxis(TEXT("Move Longitudinal"), this, &ABHPlayerController::HandleLongitudinalMovementInput);
	InputComponent->BindAxis(TEXT("Move Lateral"),this, &ABHPlayerController::HandleLateralMovementInput);
	InputComponent->BindAxis(TEXT("ZoomAxis"), this, &ABHPlayerController::HandleZoomDirectionInput);

	InputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ABHPlayerController::HandleJumpActionPressed);
	
	InputComponent->BindAction(TEXT("Sprint"),IE_Pressed, this, &ABHPlayerController::HandleSprintActionPressed);
	InputComponent->BindAction(TEXT("Sprint"),IE_Released, this, &ABHPlayerController::HandleSprintActionReleased);
	
	InputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ABHPlayerController::HandleCrouchActionPressed);
	InputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ABHPlayerController::HandleCrouchActionReleased);
	
	InputComponent->BindAction(TEXT("ToggleFlashlight"),IE_Pressed, this, &ABHPlayerController::HandleFlashlightActionPressed);
	
	InputComponent->BindAction(TEXT("PrimaryAction"), IE_Pressed, this, &ABHPlayerController::HandlePrimaryActionPressed);
	InputComponent->BindAction(TEXT("PrimaryAction"), IE_Released, this, &ABHPlayerController::HandlePrimaryActionReleased);

	InputComponent->BindAction(TEXT("SecondaryAction"), IE_Pressed, this, &ABHPlayerController::HandleSecondaryActionPressed);
	InputComponent->BindAction(TEXT("SecondaryAction"), IE_Released, this, &ABHPlayerController::HandleSecondaryActionReleased);

	InputComponent->BindAction(TEXT("TertiaryAction"), IE_Pressed, this, &ABHPlayerController::HandleTertiaryActionPressed);
	InputComponent->BindAction(TEXT("TertiaryAction"), IE_Released, this, &ABHPlayerController::HandleTertiaryActionReleased);

	InputComponent->BindAction(TEXT("InventoryAction"), IE_Pressed, this, &ABHPlayerController::HandleInventoryActionPressed);
	InputComponent->BindAction(TEXT("InventoryAction"), IE_Released, this, &ABHPlayerController::HandleInventoryActionReleased);
}

void ABHPlayerController::HandleHorizontalRotation(float Value)
{
	if (!bProcessRotationInput) { return; }
	
	if (InteractionComponent && InteractionComponent->GetIsTertiaryInteractionActive())
	{
		if (InteractionComponent->GetIsTertiaryInteractionActive())
		{
			InteractionComponent->AddYawInput(Value);
			InputRotation.X = 0;
			return;
		}
	}
	InputRotation.X = Value * CharacterConfiguration->RotationRate * 0.015;
}

void ABHPlayerController::HandleVerticalRotation(float Value)
{
	if (!bProcessRotationInput) { return; }
	
	if (InteractionComponent && InteractionComponent->GetIsTertiaryInteractionActive())
	{
		if (InteractionComponent->GetIsTertiaryInteractionActive())
		{
			InteractionComponent->AddPitchInput(Value);
			InputRotation.Y = 0;
			return;
		}
	}
	InputRotation.Y = Value * CharacterConfiguration->RotationRate * 0.015;
}

void ABHPlayerController::HandleLongitudinalMovementInput(float Value)
{
	if (!bProcessMovementInput) { return; }
		const FRotator Rotation = FRotator(0, GetControlRotation().Yaw, 0);
		GetPlayerCharacter()->AddMovementInput((Rotation.Vector()), Value);
}

void ABHPlayerController::HandleLateralMovementInput(float Value)
{
	if (!bProcessMovementInput) { return; }
		const FRotator Rotation = FRotator(0, GetControlRotation().Yaw+90, 0);
		GetPlayerCharacter()->AddMovementInput((Rotation.Vector()), Value);
}
void ABHPlayerController::HandleZoomDirectionInput(float Value)
{
	if (InteractionComponent)
	{
		InteractionComponent->AddScrollInput(Value);
	}
}

void ABHPlayerController::ProcessPlayerInput(float DeltaTime, bool bGamePaused)
{
	Super::ProcessPlayerInput(DeltaTime, bGamePaused);

	CalculateRotationMultiplier(InputRotation);
	
	AddYawInput(InputRotation.X * InteractionRotationMultiplier);
	AddPitchInput(InputRotation.Y * InteractionRotationMultiplier);

	InputRotation.Zero();
}

void ABHPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (GetControlledCharacter())
	{
		UpdatePlayerControlRotation(ControlRotation, DeltaSeconds);
	}
}

UBHPlayerInteractionComponent* ABHPlayerController::GetInteractionComponent() const
{
	return InteractionComponent;
}

ABHPlayerCharacter* ABHPlayerController::GetControlledCharacter() const
{
	return Cast<ABHPlayerCharacter>(GetPawn());
}

void ABHPlayerController::UpdatePlayerControlRotation(const FRotator& Rotation, const float DeltaSeconds)
{

	if (InteractionComponent && PlayerCharacter)
	{
		const UBHPlayerDragComponent* DragComponent = InteractionComponent->GetDragComponent();

		if (DragComponent && DragComponent->GetGrabbedComponent())
		{
			const FVector PlayerForwardVector = PlayerCharacter->GetVelocity().GetSafeNormal();
			const float PlayerForwardSpeed = static_cast<float>(FVector::DotProduct(PlayerForwardVector, PlayerCharacter->GetActorForwardVector()));
			const float PlayerSidewardSpeed = static_cast<float>(FVector::CrossProduct(PlayerForwardVector, PlayerCharacter->GetActorForwardVector()).Size());
			
			const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
			const FVector DraggedObjectLocation = DragComponent->GetDragLocation();
			const FVector CameraToDraggedObjectVector = (DraggedObjectLocation - CameraLocation).GetSafeNormal();
	
			const float DistanceToDraggedObject = static_cast<float>(FVector::Distance(CameraLocation, DraggedObjectLocation));
			const float AngleThreshold
			{static_cast<float>(FMath::GetMappedRangeValueClamped(FVector2D(80, 150), FVector2D(1, 0), DistanceToDraggedObject) * 45)};
			
			const float AngleBetweenVectors = FMath::RadiansToDegrees(acosf(FVector::DotProduct(CameraToDraggedObjectVector, GetControlRotation().Vector())));

			if (PlayerForwardSpeed <= -1 && AngleBetweenVectors > AngleThreshold)
			{
				FVector NewForwardVector {CameraToDraggedObjectVector * FMath::Cos(FMath::DegreesToRadians(AngleThreshold))
					+ GetControlRotation().Vector() * FMath::Sin(FMath::DegreesToRadians(AngleThreshold))};
				NewForwardVector.Normalize();
		
				FRotator TargetRotation = NewForwardVector.Rotation();
				TargetRotation.Pitch = ControlRotation.Pitch;
		
				ControlRotation = FMath::RInterpTo(ControlRotation, TargetRotation, DeltaSeconds, 1.5);
			}
	
			PlayerControlRotation = ControlRotation;
			return;
		}
	}
	
	if (CharacterConfiguration && CharacterConfiguration->RotationSmoothingEnabled)
	{
		PlayerControlRotation = FMath::RInterpTo(PlayerControlRotation, Rotation, DeltaSeconds, CharacterConfiguration->RotationSmoothingSpeed);
	}
	else
	{
		PlayerControlRotation = Rotation;
	}
}

void ABHPlayerController::CalculateRotationMultiplier(const FVector2D InputDirection)
{
	if (!InteractionComponent || !CharacterConfiguration)
    {
        InteractionRotationMultiplier = 1.0f;
        return;
    }

    const UBHPlayerGrabComponent* GrabComponent = InteractionComponent->GetGrabComponent();
    const UBHPlayerDragComponent* DragComponent = InteractionComponent->GetDragComponent();

    if (GrabComponent->GetGrabbedComponent() || DragComponent->GetGrabbedComponent())
    {
	    float RotationMultiplier = 1.0f;

    	if (const UPrimitiveComponent* PrimitiveComponent = GrabComponent->GetGrabbedComponent() ? GrabComponent->GetGrabbedComponent() : DragComponent->GetGrabbedComponent())
    	{
    		const float Mass = PrimitiveComponent->GetMass();
    		const FBoxSphereBounds Bounds = PrimitiveComponent->CalcBounds(PrimitiveComponent->GetComponentTransform());
    		const float BoundingBoxSize = static_cast<float>(Bounds.GetBox().GetVolume());

    		const float MassRotationMultiplier {static_cast<float>(FMath::GetMappedRangeValueClamped
				(CharacterConfiguration->InteractionRotationWeightRange, CharacterConfiguration->InteractionRotationWeightScalars, Mass))};

    		const float BoundsRotationMultiplier {static_cast<float>(FMath::GetMappedRangeValueClamped
				(CharacterConfiguration->InteractionRotationSizeRange, CharacterConfiguration->InteractionRotationSizeScalars, BoundingBoxSize))};

    		float DistanceMultiplier;

    		if (GrabComponent->GetGrabbedComponent())
    		{
    			if (const UCameraComponent* CameraComponent = PlayerCharacter->GetCamera())
    			{
    				const float Distance = static_cast<float>(FVector::Dist(PrimitiveComponent->GetComponentLocation(), CameraComponent->GetComponentLocation()));
    				
    				DistanceMultiplier = FMath::GetMappedRangeValueClamped
					(CharacterConfiguration->InteractionRotationDistanceRange, CharacterConfiguration->InteractionRotationDistanceScalars, Distance);

    				RotationMultiplier *= FMath::Clamp(MassRotationMultiplier * BoundsRotationMultiplier, CharacterConfiguration->InteractionRotationFloor, 1.0) * DistanceMultiplier;
    			}
    		}
    		else if (DragComponent->GetGrabbedComponent())
    		{
    			if (const UCameraComponent* CameraComponent = PlayerCharacter->GetCamera())
    			{
    				const FVector CameraLocation = CameraComponent->GetComponentLocation();
    				const FVector CameraForwardVector = CameraComponent->GetForwardVector();
    				const FVector DragLocation = DragComponent->GetDragLocation();
    				const FVector DistanceToComponent = DragLocation - CameraLocation;

    				const float Angle = static_cast<float>(FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(CameraForwardVector, DistanceToComponent.GetSafeNormal()))));

    				DistanceMultiplier = FMath::GetMappedRangeValueClamped(
						CharacterConfiguration->InteractionRotationOffsetRange, CharacterConfiguration->InteractionRotationOffsetScalars, Angle);

    				const FVector CameraToComponentDirection = DragComponent->GetDragLocation() - PlayerCameraManager->GetCameraLocation();
    				const FVector InputDirection3D = FVector(InputDirection, 0.0f);
    				const float DotProduct = static_cast<float>(FVector::DotProduct(InputDirection3D, CameraToComponentDirection.GetSafeNormal()));

    				constexpr float DragMultiplier = 0.2;

    				if (DotProduct > 0.0f)
    				{
    					RotationMultiplier *= FMath::Clamp(MassRotationMultiplier * BoundsRotationMultiplier, CharacterConfiguration->InteractionRotationFloor, 1.0) * DragMultiplier;
    				}
    				else
    				{
    					RotationMultiplier *= FMath::Clamp(MassRotationMultiplier * BoundsRotationMultiplier, CharacterConfiguration->InteractionRotationFloor, 1.0) * DistanceMultiplier * DragMultiplier;
    				}
    			}
    		}
    	}
    	InteractionRotationMultiplier = RotationMultiplier;
    	return;
    }
	InteractionRotationMultiplier = 1.0f;
}

bool ABHPlayerController::GetHasMovementInput() const
{
	if (InputComponent != nullptr && InteractionComponent && !InteractionComponent->GetIsTertiaryInteractionActive())
	{
		return InputComponent->GetAxisValue("Move Longitudinal") || InputComponent->GetAxisValue("Move Lateral");
	}
	return 0.0;
}

float ABHPlayerController::GetHorizontalRotationInput() const
{
	if (InputComponent != nullptr && InteractionComponent && !InteractionComponent->GetIsTertiaryInteractionActive())
	{
		return InputComponent->GetAxisValue("Horizontal Rotation");
	}
	return 0.0;
}

void ABHPlayerController::SetCanProcessMovementInput(bool bValue)
{
		bProcessMovementInput = bValue;
}

void ABHPlayerController::SetCanProcessRotationInput(bool bValue)
{
		bProcessRotationInput = bValue;
}

bool ABHPlayerController::SetMovementInputLock(bool bValue)
{
	MovementInputLockCount += bValue ? 1 : -1;
	const bool bCanProcessInput = !MovementInputLockCount;
	if (CanProcessMovementInput() != bCanProcessInput)
	{
		SetCanProcessMovementInput(bCanProcessInput);
		OnMovementInputLockChanged.Broadcast(bCanProcessInput);
	}
	return bCanProcessInput;
}

bool ABHPlayerController::SetRotationInputLock(bool bValue)
{
	RotationInputLockCount += bValue ? 1 : -1;
	const bool bCanProcessInput = !RotationInputLockCount;
	if (CanProcessRotationInput() != bCanProcessInput)
	{
		SetCanProcessRotationInput(bCanProcessInput);
		OnRotationInputLockChanged.Broadcast(bCanProcessInput);
	}
	return bCanProcessInput;
}

void ABHPlayerController::FadePlayerCameraFromBlack(float Duration)
{
	if (PlayerCharacter && PlayerCharacter->GetCameraController())
	{
		PlayerCharacter->GetCameraController()->FadeFromBlack(Duration);
	}
}

bool ABHPlayerController::CanProcessMovementInput() const
{
	return bProcessMovementInput;
}

bool ABHPlayerController::CanProcessRotationInput() const
{
	return bProcessRotationInput;
}

FRotator ABHPlayerController::GetPlayerControlRotation() const
{
	return PlayerControlRotation;
}

bool ABHPlayerController::CanCharacterSprint() const
{
	return CharacterConfiguration->IsSprintingEnabled && GetPlayerCharacter()->GetMovementComponent()->IsMovingOnGround() && !GetPlayerCharacter()->bIsCrouched
			&& GetInputAxisValue("Move Longitudinal") > 0.5 && FMath::Abs(GetInputAxisValue("Move Lateral")) <= GetInputAxisValue("Move Longitudinal");
}

FHitResult ABHPlayerController::GetCameraLookAtQuery() const
{
	constexpr float TraceLength = 250.f;
	const FVector Start = this->PlayerCameraManager->GetCameraLocation();
	const FVector End = Start + this->PlayerCameraManager->GetActorForwardVector() * TraceLength;
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	if (this->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		return HitResult;
	}
	return FHitResult();
}

void ABHPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		if (UBHStormwatchWorldSubsystem* Subsystem = World->GetSubsystem<UBHStormwatchWorldSubsystem>())
		{
			Subsystem->Unregister(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

UBHPlayerInteractionComponent* ABHPlayerController::SearchForPlayerInteractionComponent()
{
	if (!InteractionComponent)
	{
		if(GetPawn())
		{
			InteractionComponent = Cast<UBHPlayerInteractionComponent>(GetPawn()->FindComponentByClass(UBHPlayerInteractionComponent::StaticClass()));
		}
	}
	return InteractionComponent;
}



