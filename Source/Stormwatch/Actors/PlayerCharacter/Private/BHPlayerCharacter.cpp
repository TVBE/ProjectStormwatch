// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerCharacter.h"

#include "BHPlayerBodyCollisionComponent.h"
#include "BHPlayerCameraComponent.h"
#include "BHPlayerCameraController.h"
#include "BHPlayerSkeletalMeshComponent.h"
#include "BHPlayerController.h"
#include "BHPlayerDragComponent.h"
#include "BHPlayerFootCollisionComponent.h"
#include "BHPlayerGrabComponent.h"
#include "BHPlayerInteractionComponent.h"
#include "BHPlayerInventoryComponent.h"
#include "BHPlayerMovementComponent.h"
#include "BHPlayerUseComponent.h"
#include "BHStormwatchGameMode.h"
#include "..\..\..\Core\Public\BHStormwatchWorldSubystem.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Math/Vector.h"

ABHPlayerCharacter::ABHPlayerCharacter(const FObjectInitializer& ObjectInitializer)
: Super(
	ObjectInitializer
	.SetDefaultSubobjectClass<UBHPlayerMovementComponent>(CharacterMovementComponentName)
	.SetDefaultSubobjectClass<UBHPlayerSkeletalMeshComponent>(MeshComponentName)
)
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PlayerMovementComponent = Cast<UBHPlayerMovementComponent>(GetCharacterMovement());
	PlayerMesh = Cast<UBHPlayerSkeletalMeshComponent>(GetMesh());
	
	CameraController = CreateDefaultSubobject<UBHPlayerCameraController>(TEXT("Camera Controller"));
	Camera->SetRelativeLocation(FVector(22.0, 0.0, 75.0));
	CameraController->bEditableWhenInherited = true;

	Camera = CreateDefaultSubobject<UBHPlayerCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(this->CameraController);
	Camera->FieldOfView = 90.0;
	Camera->bUsePawnControlRotation = false;

	InteractionComponent = CreateDefaultSubobject<UBHPlayerInteractionComponent>(TEXT("Interaction Component"));
	InteractionComponent->bEditableWhenInherited = true;

	UseComponent = CreateDefaultSubobject<UBHPlayerUseComponent>(TEXT("Use Component"));
	UseComponent->bEditableWhenInherited = true;

	GrabComponent = CreateDefaultSubobject<UBHPlayerGrabComponent>(TEXT("Grab Component"));
	GrabComponent->bEditableWhenInherited = true;

	DragComponent = CreateDefaultSubobject<UBHPlayerDragComponent>(TEXT("Drag Component"));
	DragComponent->bEditableWhenInherited = true;

	InventoryComponent = CreateDefaultSubobject<UBHPlayerInventoryComponent>(TEXT("Inventory Component"));
	InventoryComponent->bEditableWhenInherited = true;

	BodyCollision = CreateDefaultSubobject<UBHPlayerBodyCollisionComponent>(TEXT("Body Collision"));
	BodyCollision->SetupAttachment(GetMesh());
	BodyCollision->SetRelativeLocation(FVector(0, 0, 100));
	BodyCollision->SetCapsuleHalfHeight(72);
	BodyCollision->SetCapsuleRadius(34);

	LeftFootCollision = CreateDefaultSubobject<UBHPlayerFootCollisionComponent>(TEXT("Left Foot Collision"));
	LeftFootCollision->SetupAttachment(GetMesh(), FName("foot_l_socket"));

	RightFootCollision = CreateDefaultSubobject<UBHPlayerFootCollisionComponent>(TEXT("Right Foot Collision"));
	RightFootCollision->SetupAttachment(GetMesh(), FName("foot_r_socket"));
}

void ABHPlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ensureMsgf(GetCharacterMovement()->IsA(UBHPlayerMovementComponent::StaticClass()), TEXT("GetCharacterMovement is not of class PlayerCharacterMovementComponent."));
	check(GetWorld());
	UBHStormwatchWorldSubsystem* StormwatchWorldSubsystem = GetWorld()->GetSubsystem<UBHStormwatchWorldSubsystem>();
	check(StormwatchWorldSubsystem);
	StormwatchWorldSubsystem->RegisterPlayerCharacter(this);
}

void ABHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerMovementComponent->OnLanding.AddDynamic(this, &ABHPlayerCharacter::HandleLandingStart);

	ABHStormwatchGameMode* GameMode = Cast<ABHStormwatchGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	GameMode->NotifyPlayerCharacterBeginPlay(this);

	TargetSpeed = Settings.WalkSpeed;
}

void ABHPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!NewController) { return; }
	ensureMsgf(NewController->IsA(ABHPlayerController::StaticClass()), TEXT("ABHPlayerCharacter was possessed by an invalid PlayerController."));
}

void ABHPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateYawDelta();
	UpdateRotation(DeltaTime);
	UpdateMovementSpeed();
}

void ABHPlayerCharacter::UpdateYawDelta()
{
	double Delta = GetBaseAimRotation().Yaw - GetActorRotation().Yaw;
	Delta = FRotator::NormalizeAxis(Delta);
	YawDelta = Delta;
}

void ABHPlayerCharacter::UpdateRotation(const float& DeltaTime)
{
	const UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent && ((MovementComponent->IsMovingOnGround() && abs(GetVelocity().X) > 1) || MovementComponent->bIsFalling()))
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

float ABHPlayerCharacter::CalculateTurnInPlaceRotation(const float YawDelta, const float DeltaTime, const float Factor, const float Clamp)
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

void ABHPlayerCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
}

void ABHPlayerCharacter::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);
}

void ABHPlayerCharacter::Jump()
{
	if (bJumpingEnabled)
	{
		Super::Jump();
	}
}

void ABHPlayerCharacter::StartSprinting()
{
	if (PlayerMovementComponent && !PlayerMovementComponent->GetIsSprinting())
	{
		TargetSpeed = SprintSpeed;
		PlayerMovementComponent->SetIsSprinting(true);
	}
}

void ABHPlayerCharacter::StopSprinting()
{
	if (PlayerMovementComponent && PlayerMovementComponent->GetIsSprinting())
	{
		TargetSpeed = WalkSpeed;
		PlayerMovementComponent->SetIsSprinting(false);
	}
}

bool ABHPlayerCharacter::bIsSprinting() const
{
	if (PlayerMovementComponent)
	{
		return PlayerMovementComponent->GetIsSprinting();
	}
	return false;
}

UBHPlayerCameraComponent* ABHPlayerCharacter::GetCamera() const
{
	check(Camera);
	return Camera;
}

UBHPlayerCameraController* ABHPlayerCharacter::GetCameraController() const
{
	check(CameraController);
	return CameraController;
}

UBHPlayerSkeletalMeshComponent* ABHPlayerCharacter::GetPlayerMesh() const
{
	check(PlayerMesh)
	return PlayerMesh;
}

UBHPlayerMovementComponent* ABHPlayerCharacter::GetPlayerMovementComponent() const
{
	check(PlayerMovementComponent);
	return PlayerMovementComponent;
}

UBHPlayerInteractionComponent* ABHPlayerCharacter::GetInteractionComponent() const
{
	check(InteractionComponent);
	return InteractionComponent;
}

UBHPlayerUseComponent* ABHPlayerCharacter::GetUseComponent() const
{
	check(UseComponent);
	return UseComponent;
}

UBHPlayerGrabComponent* ABHPlayerCharacter::GetGrabComponent() const
{
	check(GrabComponent);
	return GrabComponent;
}

UBHPlayerDragComponent* ABHPlayerCharacter::GetDragComponent() const
{
	check(DragComponent);
	return DragComponent;
}

UBHPlayerInventoryComponent* ABHPlayerCharacter::GetInventoryComponent() const
{
	check(InventoryComponent);
	return InventoryComponent;
}

void ABHPlayerCharacter::HandleLandingStart(EBHPlayerLandingType Value)
{
	float StunDuration = 0.0f;
	switch (Value)
	{
	case EBHPlayerLandingType::Soft:
		return;

	case EBHPlayerLandingType::Hard:
		StunDuration = 1.85f;
		break;

	case EBHPlayerLandingType::Heavy:
		StunDuration = 4.5f;
		break;
	}

	if (ABHPlayerController * PlayerController = Cast<ABHPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PlayerController->SetPlayerMovementInputLock(true);
		PlayerController->SetPlayerRotationInputLock(true);
	}
	GetWorld()->GetTimerManager().SetTimer(FallStunTimer, this, &ABHPlayerCharacter::HandleLandingEnd, StunDuration, false);
	
	GetCharacterMovement()->StopMovementImmediately();
}

void ABHPlayerCharacter::HandleLandingEnd()
{
	if (ABHPlayerController* PlayerController = Cast<ABHPlayerController>(GetController()))
	{
		PlayerController->SetPlayerMovementInputLock(false);
		PlayerController->SetPlayerRotationInputLock(false);
	}
}

float ABHPlayerCharacter::GetClearanceAbovePawn() const
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

bool ABHPlayerCharacter::CanPerformJump() const
{
	constexpr float RequiredClearance = 60;
	const float Clearance = GetClearanceAbovePawn();
	return ((Clearance > RequiredClearance || Clearance == -1.f) && bJumpingEnabled && !GetMovementComponent()->bIsFalling());
}

bool ABHPlayerCharacter::CanCrouch() const
{
	return Super::CanCrouch() && bCrouchingEnabled;
}

bool ABHPlayerCharacter::CanStandUp() const
{
	constexpr float RequiredClearance = 100;
	const float Clearance = GetClearanceAbovePawn();
	return (Clearance > RequiredClearance || Clearance == -1.f && GetMovementComponent()->bIsCrouching());
}

void ABHPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UBHStormwatchWorldSubsystem * Subsystem = GetWorld()->GetSubsystem<UBHStormwatchWorldSubsystem>())
	{
		Subsystem->UnregisterPlayerCharacter(this);
	}

	Super::EndPlay(EndPlayReason);
}
