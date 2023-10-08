// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "PlayerCharacter.h"
#include "PlayerCameraController.h"
#include "PlayerCharacterController.h"
#include "PlayerMovementComponent.h"
#include "PlayerUseComponent.h"
#include "PlayerGrabComponent.h"
#include "PlayerDragComponent.h"
#include "PlayerInventoryComponent.h"
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

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(this->RootComponent);
	Camera->SetRelativeLocation(FVector(22.0, 0.0, 75.0));
	Camera->FieldOfView = 90.0;
	Camera->bUsePawnControlRotation = false;

	CameraController = CreateDefaultSubobject<UPlayerCameraController>(TEXT("Camera Controller"));
	CameraController->bEditableWhenInherited = true;

	InteractionComponent = CreateDefaultSubobject<UPlayerInteractionComponent>(TEXT("Interaction Component"));
	InteractionComponent->bEditableWhenInherited = true;

	UseComponent = CreateDefaultSubobject<UPlayerUseComponent>(TEXT("Use Component"));
	UseComponent->bEditableWhenInherited = true;

	GrabComponent = CreateDefaultSubobject<UPlayerGrabComponent>(TEXT("Grab Component"));
	GrabComponent->bEditableWhenInherited = true;

	DragComponent = CreateDefaultSubobject<UPlayerDragComponent>(TEXT("Drag Component"));
	DragComponent->bEditableWhenInherited = true;

	InventoryComponent = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("Inventory Component"));
	InventoryComponent->bEditableWhenInherited = true;

	BodyCollision = CreateDefaultSubobject<UPlayerBodyCollisionComponent>(TEXT("Body Collision"));
	BodyCollision->SetupAttachment(GetMesh());
	BodyCollision->SetRelativeLocation(FVector(0, 0, 100));
	BodyCollision->SetCapsuleHalfHeight(72);
	BodyCollision->SetCapsuleRadius(34);

	LeftFootCollision = CreateDefaultSubobject<UPlayerFootCollisionComponent>(TEXT("Left Foot Collision"));
	LeftFootCollision->SetupAttachment(GetMesh(), FName("foot_l_socket"));

	RightFootCollision = CreateDefaultSubobject<UPlayerFootCollisionComponent>(TEXT("Right Foot Collision"));
	RightFootCollision->SetupAttachment(GetMesh(), FName("foot_r_socket"));
}

void APlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ensureMsgf(GetCharacterMovement()->IsA(UPlayerMovementComponent::StaticClass()), TEXT("GetCharacterMovement is not of class PlayerCharacterMovementComponent."));
	check(GetWorld());
	UStormwatchWorldSubsystem* StormwatchWorldSubsystem = GetWorld()->GetSubsystem<UStormwatchWorldSubsystem>();
	check(StormwatchWorldSubsystem);
	StormwatchWorldSubsystem->RegisterPlayerCharacter(this);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerMovement->OnLanding.AddDynamic(this, &APlayerCharacter::HandleLandingStart);

	AStormwatchGameMode* GameMode = Cast<AStormwatchGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	GameMode->NotifyPlayerCharacterBeginPlay(this);

	TargetSpeed = Settings.WalkSpeed;
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!NewController) { return; }
	ensureMsgf(NewController->IsA(APlayerCharacterController::StaticClass()), TEXT("APlayerCharacter was possessed by an invalid PlayerController."));
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateYawDelta();
	UpdateRotation(DeltaTime);
	UpdateMovementSpeed();
}

void APlayerCharacter::UpdateYawDelta()
{
	double Delta = GetBaseAimRotation().Yaw - GetActorRotation().Yaw;
	Delta = FRotator::NormalizeAxis(Delta);
	YawDelta = Delta;
}

void APlayerCharacter::UpdateRotation(const float& DeltaTime)
{
	const UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
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
		constexpr float YawDeltaThreshold = 30.0f;

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
	float Rotation = YawDelta * Factor * DeltaTime;
	if (abs(YawDelta) >= Clamp)
	{
		float RotationOvershoot = abs(YawDelta) - Clamp;
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

void APlayerCharacter::Jump()
{
	if (JumpingEnabled)
	{
		Super::Jump();
	}
}

void APlayerCharacter::StartSprinting()
{
	if (PlayerMovement && !PlayerMovement->GetIsSprinting())
	{
		TargetSpeed = SprintSpeed;
		PlayerMovement->SetIsSprinting(true);
	}
}

void APlayerCharacter::StopSprinting()
{
	if (PlayerMovement && PlayerMovement->GetIsSprinting())
	{
		TargetSpeed = WalkSpeed;
		PlayerMovement->SetIsSprinting(false);
	}
}

void APlayerCharacter::HandleLandingStart(EPlayerLandingType Value)
{
	float StunDuration = 0.0f;
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

	if (APlayerCharacterController * PlayerController = Cast<APlayerCharacterController>(GetWorld()->GetFirstPlayerController()))
	{
		PlayerController->SetPlayerMovementInputLock(true);
		PlayerController->SetPlayerRotationInputLock(true);
	}
	GetWorld()->GetTimerManager().SetTimer(FallStunTimer, this, &APlayerCharacter::HandleLandingEnd, StunDuration, false);
	
	GetCharacterMovement()->StopMovementImmediately();
}

void APlayerCharacter::HandleLandingEnd()
{
	if (APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(GetController()))
	{
		PlayerController->SetPlayerMovementInputLock(false);
		PlayerController->SetPlayerRotationInputLock(false);
	}
}

float APlayerCharacter::GetClearanceAbovePawn() const
{
	const FVector Start = GetActorLocation();
	const FVector End = Start + FVector(0.f, 0.f, 500.f);
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
	constexpr float RequiredClearance = 60;
	const float Clearance = GetClearanceAbovePawn();
	return ((Clearance > RequiredClearance || Clearance == -1.f) && JumpingEnabled && !GetMovementComponent()->IsFalling());
}

bool APlayerCharacter::CanCrouch() const
{
	return Super::CanCrouch() && CrouchingEnabled;
}

bool APlayerCharacter::CanStandUp() const
{
	constexpr float RequiredClearance = 100;
	const float Clearance = GetClearanceAbovePawn();
	return (Clearance > RequiredClearance || Clearance == -1.f && GetMovementComponent()->IsCrouching());
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UStormwatchWorldSubsystem * Subsystem = GetWorld()->GetSubsystem<UStormwatchWorldSubsystem>())
	{
		Subsystem->UnregisterPlayerCharacter(this);
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
