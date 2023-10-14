// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerController.h"
#include "BHPlayerCharacter.h"
#include "BHPlayerFlashlightComponent.h"
#include "BHPlayerCameraController.h"
#include "BHPlayerInteractionComponent.h"
#include "StormwatchWorldSubystem.h"
#include "BHPlayerGrabComponent.h"
#include "BHPlayerDragComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/Rotator.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PawnMovementComponent.h"

ABHPlayerCharacterController::ABHPlayerCharacterController()
{
}

void ABHPlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
}

void ABHPlayerCharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn) { return; }

	PlayerCharacter = Cast<ABHPlayerCharacter>(this->GetPawn());

	if (!PlayerCharacter) { return;  }
	
	/** Registers the controller to the player character subsystem. */
	if (const UWorld* World = GetWorld())
	{
		if (UStormwatchWorldSubsystem* PlayerSubsystem = World->GetSubsystem<UStormwatchWorldSubsystem>())
		{
			PlayerSubsystem->RegisterPlayerController(this);
		}
	}

	if (const UBHPlayerCameraController* CameraController = PlayerCharacter->GetCameraController()) 
	{
		PlayerCameraManager->ViewPitchMax = CameraController.MaximumViewPitch;
		PlayerCameraManager->ViewPitchMin = CameraController.MinimumViewPitch;
	}
}

void ABHPlayerCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent ();

	InputComponent->BindAxis(TEXT("Horizontal Rotation"), this, &ABHPlayerCharacterController::HandleHorizontalRotation);
	InputComponent->BindAxis(TEXT("Vertical Rotation"), this,  &ABHPlayerCharacterController::HandleVerticalRotation);
	InputComponent->BindAxis(TEXT("Move Longitudinal"), this, &ABHPlayerCharacterController::HandleLongitudinalMovementInput);
	InputComponent->BindAxis(TEXT("Move Lateral"),this, &ABHPlayerCharacterController::HandleLateralMovementInput);
	InputComponent->BindAxis(TEXT("ZoomAxis"), this, &ABHPlayerCharacterController::HandleZoomDirectionInput);

	InputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ABHPlayerCharacterController::HandleJumpActionPressed);
	
	InputComponent->BindAction(TEXT("Sprint"),IE_Pressed, this, &ABHPlayerCharacterController::HandleSprintActionPressed);
	InputComponent->BindAction(TEXT("Sprint"),IE_Released, this, &ABHPlayerCharacterController::HandleSprintActionReleased);
	
	InputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ABHPlayerCharacterController::HandleCrouchActionPressed);
	InputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ABHPlayerCharacterController::HandleCrouchActionReleased);
	
	InputComponent->BindAction(TEXT("ToggleFlashlight"),IE_Pressed, this, &ABHPlayerCharacterController::HandleFlashlightActionPressed);
	
	InputComponent->BindAction(TEXT("PrimaryAction"), IE_Pressed, this, &ABHPlayerCharacterController::HandlePrimaryActionPressed);
	InputComponent->BindAction(TEXT("PrimaryAction"), IE_Released, this, &ABHPlayerCharacterController::HandlePrimaryActionReleased);

	InputComponent->BindAction(TEXT("SecondaryAction"), IE_Pressed, this, &ABHPlayerCharacterController::HandleSecondaryActionPressed);
	InputComponent->BindAction(TEXT("SecondaryAction"), IE_Released, this, &ABHPlayerCharacterController::HandleSecondaryActionReleased);

	InputComponent->BindAction(TEXT("TertiaryAction"), IE_Pressed, this, &ABHPlayerCharacterController::HandleTertiaryActionPressed);
	InputComponent->BindAction(TEXT("TertiaryAction"), IE_Released, this, &ABHPlayerCharacterController::HandleTertiaryActionReleased);

	InputComponent->BindAction(TEXT("InventoryAction"), IE_Pressed, this, &ABHPlayerCharacterController::HandleInventoryActionPressed);
	InputComponent->BindAction(TEXT("InventoryAction"), IE_Released, this, &ABHPlayerCharacterController::HandleInventoryActionReleased);
}

void ABHPlayerCharacterController::HandleHorizontalRotation(float Value)
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

void ABHPlayerCharacterController::HandleVerticalRotation(float Value)
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

void ABHPlayerCharacterController::HandleLongitudinalMovementInput(float Value)
{
	if (!bProcessMovementInput) { return; }
		const FRotator Rotation = FRotator(0, GetControlRotation().Yaw, 0);
		GetPlayerCharacter()->AddMovementInput((Rotation.Vector()), Value);
}

void ABHPlayerCharacterController::HandleLateralMovementInput(float Value)
{
	if (!bProcessMovementInput) { return; }
		const FRotator Rotation = FRotator(0, GetControlRotation().Yaw+90, 0);
		GetPlayerCharacter()->AddMovementInput((Rotation.Vector()), Value);
}
void ABHPlayerCharacterController::HandleZoomDirectionInput(float Value)
{
	if (InteractionComponent)
	{
		InteractionComponent->AddScrollInput(Value);
	}
}

void ABHPlayerCharacterController::HandleJumpActionPressed()
{
	if (!bProcessMovementInput) { return; }
	if (PlayerCharacter && PlayerCharacter->CanJump())
	{
		const float Clearance = PlayerCharacter->GetClearanceAbovePawn();
		if (Clearance <= 175 && Clearance != -1.f)
		{
			// We limit the JumpZVelocity of the player under a certain clearance to prevent the character from bumping its head into the object above.
			GetPlayerCharacter()->GetCharacterMovement()->JumpZVelocity = Clearance * 4.25;
		}
		else
		{
			GetPlayerCharacter()->GetCharacterMovement()->JumpZVelocity = CharacterConfiguration->JumpVelocity;
		}
		GetPlayerCharacter()->Jump();
	}
}

void ABHPlayerCharacterController::HandleSprintActionPressed()
{
	if (!bProcessMovementInput) { return; }
	bSprintPending = true;
	if (CanCharacterSprint())
	{
		PlayerCharacter->StartSprinting();
	}
}

void ABHPlayerCharacterController::HandleSprintActionReleased()
{
	if (!bProcessMovementInput) { return; }
	bSprintPending = false;
	PlayerCharacter->StopSprinting();
}

void ABHPlayerCharacterController::HandleCrouchActionPressed()
{
	if (!bProcessMovementInput) { return; }
	
	if (CharacterConfiguration->EnableCrouchToggle)
	{
		if (!PlayerCharacter->bIsCrouched && PlayerCharacter->CanCrouch())
		{
			PlayerCharacter->Crouch(false);
			bCrouchPending = true;
			return;
		}
		if (PlayerCharacter->bIsCrouched && PlayerCharacter->CanStandUp())
		{
			PlayerCharacter->UnCrouch(false);
			bCrouchPending = false;
		}
	}
	else if (PlayerCharacter->CanCrouch())
	{
		PlayerCharacter->Crouch(false);
		bCrouchPending = true;
	}
}

void ABHPlayerCharacterController::HandleCrouchActionReleased()
{
	if (!bProcessMovementInput || CharacterConfiguration->EnableCrouchToggle) { return; }
	bCrouchPending = false;
	
	if (PlayerCharacter->bIsCrouched)
	{
		PlayerCharacter->UnCrouch(false);
	}
}

void ABHPlayerCharacterController::HandleFlashlightActionPressed()
{
	if (!PlayerCharacter || !bProcessMovementInput) { return; }
	if (UBHPlayerFlashlightComponent* Flashlight = PlayerCharacter->FindComponentByClass<UBHPlayerFlashlightComponent>())
	{
		Flashlight->SetFlashlightEnabled(!Flashlight->IsFlashlightEnabled());
	}
}

void ABHPlayerCharacterController::HandleTertiaryActionPressed()
{
	if (InteractionComponent)
	{
		InteractionComponent->BeginTertiaryInteraction();
	}
}

void ABHPlayerCharacterController::HandleTertiaryActionReleased()
{
	if (InteractionComponent)
	{
		InteractionComponent->EndTertiaryInteraction();
	}
}

void ABHPlayerCharacterController::HandlePrimaryActionPressed()
{
	if (InteractionComponent)
	{
		InteractionComponent->BeginPrimaryInteraction();
	}
}

void ABHPlayerCharacterController::HandlePrimaryActionReleased()
{
	if (InteractionComponent)
	{
		InteractionComponent->EndPrimaryInteraction();
	}
}

void ABHPlayerCharacterController::HandleSecondaryActionPressed()
{
	if (InteractionComponent)
	{
		InteractionComponent->BeginSecondaryInteraction();
	}
}

void ABHPlayerCharacterController::HandleSecondaryActionReleased()
{
	if (InteractionComponent)
	{
		InteractionComponent->EndSecondaryInteraction();
	}
}

void ABHPlayerCharacterController::HandleInventoryActionPressed()
{
	if (InteractionComponent)
	{
		InteractionComponent->BeginInventoryInteraction();
	}
}

void ABHPlayerCharacterController::HandleInventoryActionReleased()
{
}

void ABHPlayerCharacterController::ProcessPlayerInput(float DeltaTime, bool bGamePaused)
{
	CalculateRotationMultiplier(InputRotation);
	
	AddYawInput(InputRotation.X * InteractionRotationMultiplier);
	AddPitchInput(InputRotation.Y * InteractionRotationMultiplier);

	InputRotation.Zero();
	
	Super::ProcessPlayerInput(DeltaTime, bGamePaused);
}

void ABHPlayerCharacterController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (PlayerCharacter)
	{
		UpdateCurrentActions();
		UpdatePendingActions();
		UpdatePlayerControlRotation(ControlRotation, DeltaSeconds);
	}
}

void ABHPlayerCharacterController::UpdatePlayerControlRotation(const FRotator& Rotation, const float DeltaSeconds)
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

void ABHPlayerCharacterController::UpdateCurrentActions()
{
	if (!bProcessMovementInput) {return;}
	
	/** If the character is sprinting and should no longer be sprinting, stop sprinting. */
	if (!CanCharacterSprint() || !bSprintPending)
	{
		PlayerCharacter->StopSprinting();
	}
}

void ABHPlayerCharacterController::UpdatePendingActions()
{
	/** If there is a sprint pending and the character is no longer sprinting, start sprinting. */
	if (bSprintPending && !PlayerCharacter->bIsSprinting() && CanCharacterSprint())
	{
		if (!PlayerCharacter->bIsCrouched)
		{
			PlayerCharacter->StartSprinting();
		}
		/** If the character is crouching, stand up before sprinting. */
		else if (PlayerCharacter->bIsCrouched && PlayerCharacter->CanStandUp())
		{
			bCrouchPending = false;
			PlayerCharacter->UnCrouch(false);
			PlayerCharacter->StartSprinting();
		}
	}
	/** If crouch is pending and the character is not crouching, start crouching. */
	if (bCrouchPending && !PlayerCharacter->bIsCrouched && PlayerCharacter->CanCrouch())
	{
		if (PlayerCharacter->bIsSprinting())
		{
			PlayerCharacter->StopSprinting();
			bSprintPending = false;
		}
		PlayerCharacter->Crouch(false);
	}
}

void ABHPlayerCharacterController::CalculateRotationMultiplier(const FVector2D InputDirection)
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

bool ABHPlayerCharacterController::GetHasMovementInput() const
{
	if (InputComponent != nullptr && InteractionComponent && !InteractionComponent->GetIsTertiaryInteractionActive())
	{
		return InputComponent->GetAxisValue("Move Longitudinal") || InputComponent->GetAxisValue("Move Lateral");
	}
	return 0.0;
}

float ABHPlayerCharacterController::GetHorizontalRotationInput() const
{
	if (InputComponent != nullptr && InteractionComponent && !InteractionComponent->GetIsTertiaryInteractionActive())
	{
		return InputComponent->GetAxisValue("Horizontal Rotation");
	}
	return 0.0;
}

void ABHPlayerCharacterController::SetCanProcessMovementInput(bool bValue)
{
		bProcessMovementInput = bValue;
}

void ABHPlayerCharacterController::SetCanProcessRotationInput(bool bValue)
{
		bProcessRotationInput = bValue;
}

bool ABHPlayerCharacterController::SetPlayerMovementInputLock(bool bValue)
{
	MovementInputLockCount += bValue ? 1 : -1;
	const bool bCanProcessInput = !MovementInputLockCount;
	if (GetCanProcessMovementInput() != bCanProcessInput)
	{
		SetCanProcessMovementInput(bCanProcessInput);
		OnMovementInputLockChanged.Broadcast(bCanProcessInput);
	}
	return bCanProcessInput;
}

bool ABHPlayerCharacterController::SetPlayerRotationInputLock(bool bValue)
{
	RotationInputLockCount += bValue ? 1 : -1;
	const bool bCanProcessInput = !RotationInputLockCount;
	if (GetCanProcessRotationInput() != bCanProcessInput)
	{
		SetCanProcessRotationInput(bCanProcessInput);
		OnRotationInputLockChanged.Broadcast(bCanProcessInput);
	}
	return bCanProcessInput;
}

void ABHPlayerCharacterController::FadePlayerCameraFromBlack(float Duration)
{
	if (PlayerCharacter && PlayerCharacter->GetCameraController())
	{
		PlayerCharacter->GetCameraController()->FadeFromBlack(Duration);
	}
}

bool ABHPlayerCharacterController::CanCharacterSprint() const
{
	return CharacterConfiguration->IsSprintingEnabled && GetPlayerCharacter()->GetMovementComponent()->IsMovingOnGround() && !GetPlayerCharacter()->bIsCrouched
			&& GetInputAxisValue("Move Longitudinal") > 0.5 && FMath::Abs(GetInputAxisValue("Move Lateral")) <= GetInputAxisValue("Move Longitudinal");
}

FHitResult ABHPlayerCharacterController::GetCameraLookAtQuery() const
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

void ABHPlayerCharacterController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		if (UStormwatchWorldSubsystem* Subsystem = World->GetSubsystem<UStormwatchWorldSubsystem>())
		{
			Subsystem->UnregisterPlayerController(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

UBHPlayerInteractionComponent* ABHPlayerCharacterController::SearchForPlayerInteractionComponent()
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



