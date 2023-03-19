// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite.

#include "PlayerCharacterController.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterMovementComponent.h"
#include "PlayerFlashlightComponent.h"
#include "LogCategories.h"
#include "PlayerCameraController.h"
#include "PlayerSubsystem.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Math/Rotator.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

/** Called on construction. */
APlayerCharacterController::APlayerCharacterController()
{
}

/** Called when the game starts. */
void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
	
	/** Get a pointer to the PlayerCharacter instance this controller is controlling. */
	if(!GetPawn())
	{
		UE_LOG(LogPlayerCharacterController, Warning, TEXT("PlayerCharacterController is not assigned to a pawn."));
		return;
	}
	PlayerCharacter = Cast<APlayerCharacter>(this->GetPawn());
	if(PlayerCharacter)
	{
		if(PlayerCharacter->GetCharacterConfiguration())
		{
			CharacterConfiguration = PlayerCharacter->GetCharacterConfiguration();	
		}
	}
	else
	{
		const FString PawnName {UKismetSystemLibrary::GetDisplayName(GetPawn())};
		UE_LOG(LogPlayerCharacterController, Warning, TEXT("PlayerCharacterController expected a Pawn of type PlayerCharacter, but got assigned to an instance of %s instead"), *PawnName);
	}
}

/** Called when the controller possesses a pawn. */
void APlayerCharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	/** Registers the controller to the player character subsystem. */
	if(const UWorld* World {GetWorld()})
	{
		if(UPlayerSubsystem* PlayerSubsystem {World->GetSubsystem<UPlayerSubsystem>()})
		{
			PlayerSubsystem->RegisterPlayerController(this);
		}
	}
	
	if(const UPlayerCameraController* CameraController {Cast<UPlayerCameraController>(InPawn->FindComponentByClass(UPlayerCameraController::StaticClass()))})
	{
		if(const UPlayerCameraConfiguration* Configuration {CameraController->GetConfiguration()})
		{
			PlayerCameraManager->ViewPitchMax = Configuration->MaximumViewPitch;
			PlayerCameraManager->ViewPitchMin = Configuration->MinimumViewPitch;
		}
	}
}

/** Called when the controller is constructed. */
void APlayerCharacterController::SetupInputComponent()
{
	Super :: SetupInputComponent ();

	InputComponent->BindAxis(TEXT("Horizontal Rotation"), this, &APlayerCharacterController::HandleHorizontalRotation);
	InputComponent->BindAxis(TEXT("Vertical Rotation"), this,  &APlayerCharacterController::HandleVerticalRotation);
	InputComponent->BindAxis(TEXT("Move Longitudinal"), this, &APlayerCharacterController::HandleLongitudinalMovementInput);
	InputComponent->BindAxis(TEXT("Move Lateral"),this, &APlayerCharacterController::HandleLateralMovementInput);

	InputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &APlayerCharacterController::HandleJumpActionPressed);
	InputComponent->BindAction(TEXT("Sprint"),IE_Pressed, this, &APlayerCharacterController::HandleSprintActionPressed);
	InputComponent->BindAction(TEXT("Sprint"),IE_Released, this, &APlayerCharacterController::HandleSprintActionReleased);
	InputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &APlayerCharacterController::HandleCrouchActionPressed);
	InputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &APlayerCharacterController::HandleCrouchActionReleased);
	InputComponent->BindAction(TEXT("ToggleFlashlight"),IE_Pressed, this, &APlayerCharacterController::HandleFlashlightActionPressed);
}

void APlayerCharacterController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(PlayerCharacter)
	{
		if(const UPlayerCharacterMovementComponent* CharacterMovement {PlayerCharacter->GetPlayerCharacterMovement()})
		{
			UpdateCurrentActions(CharacterMovement);
			UpdatePendingActions(CharacterMovement);
		}
	}
	UpdatePlayerControlRotation(ControlRotation, DeltaSeconds);
}

void APlayerCharacterController::UpdatePlayerControlRotation(const FRotator& Rotation, const float DeltaSeconds)
{
	PlayerControlRotation = FMath::RInterpTo(PlayerControlRotation, Rotation, DeltaSeconds, ControlInterpolationSpeed);
}

void APlayerCharacterController::UpdateCurrentActions(const UPlayerCharacterMovementComponent* CharacterMovement)
{
	if(!CanProcessMovementInput) {return;}
	/** If the character is sprinting and should no longer be sprinting, stop sprinting. */
	if(CharacterMovement->GetIsSprinting() && !CanCharacterSprint())
	{
		StopSprinting();
	}
	/** If the character is sprinting but sprinting is no longer pending, stop sprinting. */
	if(CharacterMovement->GetIsSprinting() && !IsSprintPending)
	{
		StopSprinting();
	}
}

void APlayerCharacterController::UpdatePendingActions(const UPlayerCharacterMovementComponent* CharacterMovement)
{
	/** If there is a sprint pending and the character is no longer sprinting, start sprinting. */
	if(IsSprintPending && !CharacterMovement->GetIsSprinting() && CanCharacterSprint())
	{
		if(!CharacterMovement->IsCrouching())
		{
			StartSprinting();
		}
		/** If the character is crouching, stand up before sprinting. */
		else if(CharacterMovement->IsCrouching() && PlayerCharacter->CanStandUp())
		{
			StopCrouching();
			StartSprinting();
			IsCrouchPending = false;
		}
	}
	/** If crouch is pending and the character is not crouching, start crouching. */
	if(IsCrouchPending && !CharacterMovement->IsCrouching() && PlayerCharacter->CanCrouch())
	{
		if(CharacterMovement->GetIsSprinting())
		{
			StopSprinting();
			IsSprintPending = false;
		}
		StartCrouching();
	}
}

void APlayerCharacterController::HandleHorizontalRotation(float Value)
{
	if(!CanProcessRotationInput) {return;}
	AddYawInput(Value * CharacterConfiguration->RotationRate * 0.015);
}

void APlayerCharacterController::HandleVerticalRotation(float Value)
{
	if(!CanProcessRotationInput) {return;}
		AddPitchInput(Value * CharacterConfiguration->RotationRate * 0.015);
}

void APlayerCharacterController::HandleLongitudinalMovementInput(float Value)
{
	if(!CanProcessMovementInput) {return;}
		const FRotator Rotation {FRotator(0, GetControlRotation().Yaw, 0)};
		GetCharacter()->AddMovementInput((Rotation.Vector()), Value);
}

void APlayerCharacterController::HandleLateralMovementInput(float Value)
{
	if(!CanProcessMovementInput) {return;}
		const FRotator Rotation {FRotator(0, GetControlRotation().Yaw+90, 0)};
		GetCharacter()->AddMovementInput((Rotation.Vector()), Value);
}

void APlayerCharacterController::HandleJumpActionPressed()
{
	if(!CanProcessMovementInput) {return;}
	if(PlayerCharacter && PlayerCharacter->CanJump())
	{
		const float Clearance = PlayerCharacter->GetClearanceAbovePawn();
		if(Clearance <= 175 && Clearance != -1.f)
		{
			// We limit the JumpZVelocity of the player under a certain clearance to prevent the character from bumping its head into the object above.
			GetCharacter()->GetCharacterMovement()->JumpZVelocity = Clearance * 4.25;
		}
		else
		{
			GetCharacter()->GetCharacterMovement()->JumpZVelocity = CharacterConfiguration->JumpVelocity;
		}
		GetCharacter()->Jump();
	}
}

void APlayerCharacterController::HandleSprintActionPressed()
{
	if(!CanProcessMovementInput) {return;}
	IsSprintPending = true;
	if(CanCharacterSprint())
	{
		StartSprinting();
	}
}


void APlayerCharacterController::HandleSprintActionReleased()
{
	if(!CanProcessMovementInput) {return;}
	IsSprintPending = false;
	if(PlayerCharacter->GetPlayerCharacterMovement() && PlayerCharacter->GetPlayerCharacterMovement()->GetIsSprinting())
	{
		StopSprinting();
	}
}

void APlayerCharacterController::HandleCrouchActionPressed()
{
	if(!CanProcessMovementInput) {return;}
	IsCrouchPending = true;

	if(CharacterConfiguration->EnableCrouchToggle)
	{
		if(!GetCharacter()->GetMovementComponent()->IsCrouching() && PlayerCharacter->CanCrouch())
		{
			StartCrouching();
			return;
		}
		if(GetCharacter()->GetMovementComponent()->IsCrouching() && PlayerCharacter->CanStandUp())
		{
			StopCrouching();
		}
	}
	else if(PlayerCharacter->CanCrouch())
	{
		StartCrouching();
	}
}

void APlayerCharacterController::HandleCrouchActionReleased()
{
	if(!CanProcessMovementInput) {return;}
	IsCrouchPending = false;
}

void APlayerCharacterController::HandleFlashlightActionPressed()
{
	if(!PlayerCharacter || !CanProcessMovementInput) {return; }
	if(UPlayerFlashlightComponent* Flashlight = PlayerCharacter->FindComponentByClass<UPlayerFlashlightComponent>())
	{
		Flashlight->SetFlashlightEnabled(!Flashlight->IsFlashlightEnabled());
	}
}

bool APlayerCharacterController::GetHasMovementInput() const
{
	if(InputComponent != nullptr)
	{
		return InputComponent->GetAxisValue("Move Longitudinal") || InputComponent->GetAxisValue("Move Lateral");
	}
	return 0.0;
}

float APlayerCharacterController::GetHorizontalRotationInput() const
{
	if(InputComponent != nullptr)
	{
		return InputComponent->GetAxisValue("Horizontal Rotation");
	}
	return 0.0;
}

void APlayerCharacterController::SetCanProcessMovementInput(const UPlayerSubsystem* Subsystem, const bool Value)
{
	if(Subsystem)
	{
		CanProcessMovementInput = Value;
	}
}

void APlayerCharacterController::SetCanProcessRotationInput(const UPlayerSubsystem* Subsystem, const bool Value)
{
	if(Subsystem)
	{
		CanProcessRotationInput = Value;
	}
}

bool APlayerCharacterController::CanCharacterSprint() const
{
	return CharacterConfiguration->IsSprintingEnabled && GetCharacter()->GetMovementComponent()->IsMovingOnGround()
			&& GetInputAxisValue("Move Longitudinal") > 0.5 && FMath::Abs(GetInputAxisValue("Move Lateral")) <= GetInputAxisValue("Move Longitudinal");
}

bool APlayerCharacterController::CanInteract() const
{
	return false; // Temp
}

void APlayerCharacterController::StartSprinting()
{
	GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = CharacterConfiguration->SprintSpeed;
	if(UPlayerCharacterMovementComponent* PlayerCharacterMovement {PlayerCharacter->GetPlayerCharacterMovement()})
	{
		PlayerCharacterMovement->SetIsSprinting(true, this);	
	}
}

void APlayerCharacterController::StopSprinting()
{
	GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = CharacterConfiguration->WalkSpeed;
	if(UPlayerCharacterMovementComponent* PlayerCharacterMovement {PlayerCharacter->GetPlayerCharacterMovement()})
	{
		PlayerCharacterMovement->SetIsSprinting(false, this);	
	}
}

void APlayerCharacterController::StartCrouching()
{
	GetCharacter()->Crouch();
}

void APlayerCharacterController::StopCrouching()
{
	// Temp
}

FHitResult APlayerCharacterController::GetCameraLookAtQuery() const
{
	constexpr float TraceLength {250.f};
	const FVector Start {this->PlayerCameraManager->GetCameraLocation()};
	const FVector End {Start + this->PlayerCameraManager->GetActorForwardVector() * TraceLength};
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	if (this->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		return HitResult;
	}
	return FHitResult();
}

void APlayerCharacterController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(const UWorld* World {GetWorld()})
	{
		if(UPlayerSubsystem* Subsystem {World->GetSubsystem<UPlayerSubsystem>()})
		{
			Subsystem->UnregisterPlayerController(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}


