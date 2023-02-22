// Copyright 2023 Tim Verberne.


#include "PlayerCharacterController.h"
#include "Frostbite/Frostbite.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterMovementComponent.h"
#include "PlayerCharacterState.h"
#include "Math/Rotator.h"
#include "PlayerFlashlightController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Called on construction
APlayerCharacterController::APlayerCharacterController()
{
}

// Called when the game starts.
void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
	
	// Get a casted reference to the PlayerCharacter
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

	// Start updating the player state.
	if(GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(StateTimer, this, &APlayerCharacterController::UpdatePlayerState, 1.0f, true);	
	}
}

// Called when the PlayerState is constructed.
void APlayerCharacterController::InitPlayerState()
{
	Super::InitPlayerState();
	PlayerCharacterState = Cast<APlayerCharacterState>(PlayerState);
	if(!PlayerCharacterState)
	{
		UE_LOG(LogPlayerCharacterController, Error, TEXT("Player state is not an instance of PlayerCharacterState. "));
	}
}

// Called when the controller is constructed.
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
	UpdateCurrentActions();
	UpdatePendingActions();
}


void APlayerCharacterController::HandleHorizontalRotation(float Value)
{
	if(!CanProcessRotationInput) {return;}
	if(CanRotate())
	{
		AddYawInput(Value * CharacterConfiguration->RotationRate * 0.015);
	}
}

void APlayerCharacterController::HandleVerticalRotation(float Value)
{
	if(!CanProcessRotationInput) {return;}
	if(CanRotate())
	{
		AddPitchInput(Value * CharacterConfiguration->RotationRate * 0.015);
	}
}

void APlayerCharacterController::HandleLongitudinalMovementInput(float Value)
{
	if(!CanProcessMovementInput) {return;}
	if(CanMove())
	{
		const FRotator Rotation {FRotator(0, GetControlRotation().Yaw, 0)};
		GetCharacter()->AddMovementInput((Rotation.Vector()), Value);
	}
}

void APlayerCharacterController::HandleLateralMovementInput(float Value)
{
	if(!CanProcessMovementInput) {return;}
	if(CanMove())
	{
		const FRotator Rotation {FRotator(0, GetControlRotation().Yaw+90, 0)};
		GetCharacter()->AddMovementInput((Rotation.Vector()), Value);
	}
}

void APlayerCharacterController::HandleJumpActionPressed()
{
	if(!CanProcessMovementInput) {return;}
	if(CanJump())
	{
		const float Clearance = GetClearanceAbovePawn();
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
	if(CanSprint())
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
		if(!GetCharacter()->GetMovementComponent()->IsCrouching() && CanCrouch())
		{
			StartCrouching();
			return;
		}
		if(GetCharacter()->GetMovementComponent()->IsCrouching() && CanStandUp())
		{
			StopCrouching();
		}
	}
	else if(CanCrouch())
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
	if(!CanProcessMovementInput) {return;}
	if(CanToggleFlashlight())
	{
		PlayerCharacter->GetFlashlightController()->SetFlashlightEnabled(!PlayerCharacter->GetFlashlightController()->IsFlashlightEnabled());
	}
}

void APlayerCharacterController::UpdateCurrentActions()
{
	if(!CanProcessMovementInput) {return;}
	if(PlayerCharacter->GetPlayerCharacterMovement() && PlayerCharacter->GetPlayerCharacterMovement()->GetIsSprinting() && !CanSprint())
	{
		StopSprinting();
	}
	if(PlayerCharacter->GetPlayerCharacterMovement() && PlayerCharacter->GetPlayerCharacterMovement()->GetIsSprinting() && !IsSprintPending)
	{
		StopSprinting();
	}
}


void APlayerCharacterController::UpdatePendingActions()
{
	if(IsSprintPending && PlayerCharacter->GetPlayerCharacterMovement() && !PlayerCharacter->GetPlayerCharacterMovement()->GetIsSprinting() && CanSprint())
	{
		if(!GetCharacter()->GetMovementComponent()->IsCrouching())
		{
			StartSprinting();
		}
		else if(GetCharacter()->GetMovementComponent()->IsCrouching() && CanStandUp())
		{
			StopCrouching();
			StartSprinting();
			IsCrouchPending = false;
		}
	}
	if(IsCrouchPending && !GetCharacter()->GetMovementComponent()->IsCrouching() && CanCrouch())
	{
			if(PlayerCharacter->GetPlayerCharacterMovement() && PlayerCharacter->GetPlayerCharacterMovement()->GetIsSprinting())
			{
				StopSprinting();
				IsSprintPending = false;
			}
			StartCrouching();
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

bool APlayerCharacterController::CanRotate() const
{
	return true;
}

bool APlayerCharacterController::CanMove() const
{
	return true; // Temp
}

bool APlayerCharacterController::CanJump() const
{
	constexpr float RequiredClearance {60};
	const float Clearance {GetClearanceAbovePawn()};
	return ((Clearance > RequiredClearance || Clearance == -1.f) && CharacterConfiguration->IsJumpingEnabled && !GetCharacter()->GetMovementComponent()->IsFalling());
}

bool APlayerCharacterController::CanSprint() const
{
	return CharacterConfiguration->IsSprintingEnabled && GetCharacter()->GetMovementComponent()->IsMovingOnGround()
			&& GetInputAxisValue("Move Longitudinal") > 0.5 && FMath::Abs(GetInputAxisValue("Move Lateral")) <= GetInputAxisValue("Move Longitudinal");
}

bool APlayerCharacterController::CanCrouch() const
{
	return CharacterConfiguration->IsCrouchingEnabled;
}

bool APlayerCharacterController::CanInteract() const
{
	return false; // Temp
}

bool APlayerCharacterController::CanToggleFlashlight() const
{
	if(PlayerCharacter && PlayerCharacter->GetFlashlightController())
	{
		return true;
	}
	if(!PlayerCharacter->GetFlashlightController())
	{
		UE_LOG(LogPlayerCharacterController, Error, TEXT("Couldn't find flashlight controller"))
	}
	return false;
}


bool APlayerCharacterController::CanStandUp() const
{
	constexpr float RequiredClearance {100};
	const float Clearance {GetClearanceAbovePawn()};
	return (Clearance > RequiredClearance || Clearance == -1.f && GetCharacter()->GetMovementComponent()->IsCrouching());
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

float APlayerCharacterController::GetClearanceAbovePawn() const
{
	const AActor* Actor {this->GetPawn()};
	const FVector Start {Actor->GetActorLocation()};
	const FVector End {Start + FVector(0.f, 0.f, 500.f)};
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	if (Actor->GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		return HitResult.Distance - GetCharacter()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		// We subtract the capsule collision half height as this is the distance between the center of the SkeletalMesh and the top of the head.
	}
	
	return -1.f; // We return -1 if no hit result is produced by the collision query. This means that there is more than 500 units of clearance above the character.
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

void APlayerCharacterController::UpdatePlayerState()
{
	if(PlayerCharacter && PlayerCharacterState)
	{
		if(PlayerCharacterState->GetHealth() < 100)
		{
			PlayerCharacterState->IncrementHealth(4);
		}
		if(const UPlayerCharacterMovementComponent* PlayerCharacterMovement {PlayerCharacter->GetPlayerCharacterMovement()})
		{
			if(PlayerCharacterMovement->GetIsSprinting())
			{
				PlayerCharacterState->IncrementExertion(3);
			}
			else
			{
				PlayerCharacterState->DecrementExertion(2);
			}
		}
		if(PlayerCharacterState->GetFear() > 0)
		{
			PlayerCharacterState->DecrementFear(1);
		}
		if(PlayerCharacterState->GetVigilance() > 0)
		{
			PlayerCharacterState->DecrementVigilance(1);
		}
	}
}


