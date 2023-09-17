// Copyright (c) 2022-present Barrelhouse. All rights reserved.
// Written by Tim Verberne.

#include "PlayerCharacter.h"
#include "PlayerCameraController.h"
#include "PlayerCharacterController.h"
#include "PlayerMovementComponent.h"
#include "StormwatchWorldSubystem.h"
#include "StormwatchGameMode.h"
#include "PlayerBodyCollisionComponent.h"
#include "PlayerFootCollisionComponent.h"
#include "PlayerInteractionComponent.h"
#include "PlayerGrabComponent.h"
#include "PlayerDragComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Math/Vector.h"

DEFINE_LOG_CATEGORY_CLASS(APlayerCharacter, LogPlayerCharacter);

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/** Construct camera. */
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(this->RootComponent);
	Camera->SetRelativeLocation(FVector(22.0, 0.0, 75.0));
	Camera->FieldOfView = 90.0;
	Camera->bUsePawnControlRotation = false;

	/** Construct camera controller. */
	CameraController = CreateDefaultSubobject<UPlayerCameraController>(TEXT("Camera Controller"));
	CameraController->bEditableWhenInherited = true;

	/** Construct body collision component. */
	BodyCollision = CreateDefaultSubobject<UPlayerBodyCollisionComponent>(TEXT("Body Collision"));
	BodyCollision->SetupAttachment(GetMesh());
	BodyCollision->SetRelativeLocation(FVector(0, 0, 100));
	BodyCollision->SetCapsuleHalfHeight(72);
	BodyCollision->SetCapsuleRadius(34);

	/** Construct foot collision components. */
	LeftFootCollision = CreateDefaultSubobject<UPlayerFootCollisionComponent>(TEXT("Left Foot Collision"));
	LeftFootCollision->SetupAttachment(GetMesh(), FName("foot_l_socket"));

	RightFootCollision = CreateDefaultSubobject<UPlayerFootCollisionComponent>(TEXT("Right Foot Collision"));
	RightFootCollision->SetupAttachment(GetMesh(), FName("foot_r_socket"));

	/** Construct interaction component. */
	InteractionComponent = CreateDefaultSubobject<UPlayerInteractionComponent>(TEXT("Interaction Component"));
	CameraController->bEditableWhenInherited = true;
}

void APlayerCharacter::Jump()
{
	if (Settings.JumpingEnabled)
	{
		Super::Jump();
	}
}

/** Called after the constructor but before the components are initialized. */
void APlayerCharacter::PostInitProperties()
{
	ValidateConfigurationAssets();

	if (UPlayerMovementComponent * PlayerMovementComponent {Cast<UPlayerMovementComponent>(GetCharacterMovement())})
	{
		PlayerMovement = PlayerMovementComponent;
	}

	/** Set components to call their virtual InitializeComponent functions. */
	CameraController->bWantsInitializeComponent = true;

	Super::PostInitProperties();
}

/** Called after all default property values have been fully initialized, but before any of the components are initialized. */
void APlayerCharacter::OnConstruction(const FTransform& Transform)
{
	/** Registers this player character to the player character subsystem. */
	if (const UWorld * World {GetWorld()})
	{
		if (UStormwatchWorldSubsystem * PlayerSubsystem {World->GetSubsystem<UStormwatchWorldSubsystem>()})
		{
			PlayerSubsystem->RegisterPlayerCharacter(this);
		}
	}

	Super::OnConstruction(Transform);
}

/** Called after InitializeComponents. */
void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ApplyConfigurationAssets();

	/** Subscribe to the OnLanding event of the player character movement component. */
	if (PlayerMovement)
	{
		PlayerMovement->OnLanding.AddDynamic(this, &APlayerCharacter::HandleLanding);
	}
}

/** Called when the game starts or when spawned. */
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	/** Notify the GameMode that the character has Begun Play. */
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		if (AStormwatchGameMode * GameMode {Cast<AStormwatchGameMode>(GetWorld()->GetAuthGameMode())})
		{
			GameMode->NotifyPlayerCharacterBeginPlay(this);
		}
	}
		TargetSpeed = Settings.WalkSpeed;
}

/** Called when the controller is changed. */
void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController)
	{
		PlayerCharacterController = Cast<APlayerCharacterController>(NewController);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateYawDelta();
	UpdateRotation(DeltaTime);
	UpdateMovementSpeed();
}

void APlayerCharacter::UpdateMovementSpeed()
{
	float InteractionMultiplier {1.0f};

	if (!InteractionComponent || !PlayerMovement) { return; }

	const UPlayerGrabComponent* GrabComponent {InteractionComponent->GetGrabComponent()};
	const UPlayerDragComponent* DragComponent {InteractionComponent->GetDragComponent()};

	if (!GrabComponent->GetGrabbedComponent() || !DragComponent->GetGrabbedComponent()) { return; }

	const UPrimitiveComponent* PrimitiveComponent {GrabComponent->GetGrabbedComponent() ? GrabComponent->GetGrabbedComponent() : DragComponent->GetGrabbedComponent()};
	
	const float Mass {PrimitiveComponent->GetMass()};
	const FBoxSphereBounds Bounds {PrimitiveComponent->CalcBounds(PrimitiveComponent->GetComponentTransform())};
	const float BoundingBoxSize {static_cast<float>(Bounds.GetBox().GetVolume())};

	const float MassRotationMultiplier {static_cast<float>(FMath::GetMappedRangeValueClamped
		(Settings.InteractionSpeedWeightRange, Settings.InteractionSpeedWeightScalars, Mass))};
	const float BoundsRotationMultiplier {static_cast<float>(FMath::GetMappedRangeValueClamped
		(Settings.InteractionSpeedSizeRange, Settings.InteractionSpeedSizeScalars, BoundingBoxSize))};

	InteractionMultiplier *= FMath::Clamp(MassRotationMultiplier * BoundsRotationMultiplier, 
										  Settings.InteractionSpeedFloor, 1.0);

	PlayerMovement->MaxWalkSpeed = ScaledSpeed;
	PlayerMovement->MaxWalkSpeedCrouched = Settings.CrouchSpeed * InteractionMultiplier; // TODO: Needs different implementation in future.
}

void APlayerCharacter::UpdateYawDelta()
{
	double Delta {GetBaseAimRotation().Yaw - GetActorRotation().Yaw};
	Delta = FRotator::NormalizeAxis(Delta);
	YawDelta = Delta;
}

void APlayerCharacter::UpdateRotation(const float& DeltaTime)
{
	const UCharacterMovementComponent* MovementComponent {GetCharacterMovement()};
	if (MovementComponent && ((MovementComponent->IsMovingOnGround() && abs(GetVelocity().X) > 1) || MovementComponent->IsFalling()))
	{
		if (GetController())
		{
			SetActorRotation(FRotator(0, PlayerCharacterController->GetPlayerControlRotation().Yaw, 0));
		}
		IsTurningInPlace = false;
	}
	else
	{
		constexpr float YawDeltaThreshold {30.0f};

		if (IsTurningInPlace)
		{
			AddActorWorldRotation(FRotator(0, CalculateTurnInPlaceRotation(YawDelta, DeltaTime, 4.f, 45.0f), 0));
		}
		if (FMath::IsNearlyEqual(YawDelta, 0, 0.5f))
		{
			IsTurningInPlace = false;
		}
		else if (abs(YawDelta) > YawDeltaThreshold)
		{
			IsTurningInPlace = true;
		}
	}
}

float APlayerCharacter::CalculateTurnInPlaceRotation(const float YawDelta, const float DeltaTime, const float Factor, const float Clamp)
{
	float Rotation {YawDelta * Factor * DeltaTime};
	if (abs(YawDelta) >= Clamp)
	{
		float RotationOvershoot {abs(YawDelta) - Clamp};
		RotationOvershoot = (YawDelta >= 0.0) ? RotationOvershoot : -RotationOvershoot;
		Rotation += RotationOvershoot;
	}
	return Rotation;
}

void APlayerCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
}

void APlayerCharacter::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);
}

void APlayerCharacter::StartSprinting()
{
	if (PlayerMovement && !PlayerMovement->GetIsSprinting())
	{
		TargetSpeed = Settings.SprintSpeed;
		PlayerMovement->SetIsSprinting(true);
	}
}

void APlayerCharacter::StopSprinting()
{
	if (PlayerMovement && PlayerMovement->GetIsSprinting())
	{
		TargetSpeed = Settings.WalkSpeed;
		PlayerMovement->SetIsSprinting(false);
	}
}

void APlayerCharacter::HandleLanding(EPlayerLandingType Value)
{
	float StunDuration {0.0f};
	switch (Value)
	{
	case EPlayerLandingType::Soft:
		return;

	case EPlayerLandingType::Hard:
		StunDuration = 1.85f;
		break;

	case EPlayerLandingType::Heavy:
		StunDuration = 4.5f;
		break;
	}

	if (const UWorld * World {GetWorld()})
	{
		if (APlayerCharacterController * PlayerController {Cast<APlayerCharacterController>(GetWorld()->GetFirstPlayerController())})
		{
			PlayerController->SetPlayerMovementInputLock(true);
			PlayerController->SetPlayerRotationInputLock(true);
		}
		GetWorld()->GetTimerManager().SetTimer(FallStunTimer, this, &APlayerCharacter::HandleLandingEnd, StunDuration, false);
	}
	GetCharacterMovement()->StopMovementImmediately();
}

void APlayerCharacter::HandleLandingEnd()
{
	if (PlayerCharacterController)
	{
		if (const UWorld * World {GetWorld()})
		{
			if (APlayerCharacterController * PlayerController {Cast<APlayerCharacterController>(GetWorld()->GetFirstPlayerController())})
			{
				PlayerController->SetPlayerMovementInputLock(false);
				PlayerController->SetPlayerRotationInputLock(false);
			}
		}
	}
}

float APlayerCharacter::GetClearanceAbovePawn() const
{
	const FVector Start {GetActorLocation()};
	const FVector End {Start + FVector(0.f, 0.f, 500.f)};
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		/** We subtract the capsule collision half height as this is the distance between the center of the SkeletalMesh and the top of the head. */
		return HitResult.Distance - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}

	/** We return -1 if no hit result is produced by the collision query. This means that there is more than 500 units of clearance above the character. */
	return -1.f;
}

bool APlayerCharacter::CanPerformJump() const
{
	constexpr float RequiredClearance {60};
	const float Clearance {GetClearanceAbovePawn()};
	return ((Clearance > RequiredClearance || Clearance == -1.f) && Settings.JumpingEnabled && !GetMovementComponent()->IsFalling());
}

bool APlayerCharacter::CanCrouch() const
{
	return Super::CanCrouch() && Settings.CrouchingEnabled;
}

bool APlayerCharacter::CanStandUp() const
{
	constexpr float RequiredClearance {100};
	const float Clearance {GetClearanceAbovePawn()};
	return (Clearance > RequiredClearance || Clearance == -1.f && GetMovementComponent()->IsCrouching());
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld * World {GetWorld()})
	{
		if (UStormwatchWorldSubsystem * Subsystem {World->GetSubsystem<UStormwatchWorldSubsystem>()})
		{
			Subsystem->UnregisterPlayerCharacter(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

//void UPlayerCharacterSettings::Apply(APlayerCharacter* Character)
//{
//	if (!Character) { return; }
//	if (UCharacterMovementComponent * MovementComponent {Character->GetCharacterMovement()})
//	{
//		MovementComponent->MaxWalkSpeed = Settings.WalkSpeed;
//		MovementComponent->JumpZVelocity = Settings.JumpVelocity;
//		MovementComponent->MaxWalkSpeedCrouched = Settings.CrouchSpeed;
//		MovementComponent->SetWalkableFloorAngle(Settings.MaxWalkableFloorAngle);
//		MovementComponent->MaxStepHeight = Settings.MaxStepHeight;
//	}
//}
