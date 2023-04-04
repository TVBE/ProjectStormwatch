// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "PlayerCharacter.h"
#include "PlayerCameraController.h"
#include "PlayerCharacterController.h"
#include "PlayerCharacterMovementComponent.h"
#include "PlayerSubsystem.h"
#include "FrostbiteGameMode.h"
#include "LogCategories.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Math/Vector.h"

/** The PlayerCharacter's initialization follows these stages:
 *	1) Constructor: Creates the actor and sets its default properties. We cannot access default property values at this time.
 *	2) PostInitProperties(): Called after construction to perform additional initialization that requires access to default property values.
 *	3) OnConstruction(): Called after all default property values have been fully initialized, but before any of the components are initialized.
 *	4) PostInitializeComponents(): Called after initializing the components, which allows them to register with other systems and set up data structures.
 *	5) BeginPlay(): Called when the actor is ready to be used in the game world.
 */

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/** Construct Camera. */
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(this->RootComponent);
	Camera->SetRelativeLocation(FVector(22.0, 0.0, 75.0));
	Camera->FieldOfView = 90.0;
	Camera->bUsePawnControlRotation = false;
	
	/** Construct Camera Controller. */
	CameraController = CreateDefaultSubobject<UPlayerCameraController>(TEXT("Camera Controller"));
	CameraController->bEditableWhenInherited = true;
}

/** Called after the constructor but before the components are initialized. */
void APlayerCharacter::PostInitProperties()
{
	ValidateConfigurationAssets();
	
	if (UPlayerCharacterMovementComponent* PlayerCharacterMovementComponent {Cast<UPlayerCharacterMovementComponent>(GetCharacterMovement())})
	{
		PlayerCharacterMovement = PlayerCharacterMovementComponent;
	}
	
	/** Set components to call their virtual InitializeComponent functions. */
	CameraController->bWantsInitializeComponent = true;
	
	Super::PostInitProperties();
}

/** Called after all default property values have been fully initialized, but before any of the components are initialized. */
void APlayerCharacter::OnConstruction(const FTransform& Transform)
{
	/** Registers this player character to the player character subsystem. */
	if (const UWorld* World {GetWorld()})
	{
		if (UPlayerSubsystem* PlayerSubsystem {World->GetSubsystem<UPlayerSubsystem>()})
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
	if (PlayerCharacterMovement)
	{
		PlayerCharacterMovement->OnLanding.AddDynamic(this, &APlayerCharacter::HandleLanding);
	}
}

/** Called when the game starts or when spawned. */
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	/** Notify the GameMode that the character has Begun Play. */
	if (GetWorld() && GetWorld()->GetAuthGameMode())
	{
		if (AFrostbiteGameMode* GameMode {Cast<AFrostbiteGameMode>(GetWorld()->GetAuthGameMode())})
		{
			GameMode->NotifyPlayerCharacterBeginPlay(this);
		}
	}

#if WITH_EDITOR
		/** Check if all components have been succesfully initialized. */
		ValidateObject(CameraController, "CameraController");
#endif
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

/** Called every frame. */
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateYawDelta();
	UpdateRotation(DeltaTime);
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

#if WITH_EDITOR
void APlayerCharacter::ValidateObject(const UObject* Object, const FString ObjectName)
{
	if (!Object)
	{
		UE_LOG(LogPlayerCharacter, Error, TEXT("%s was not properly initialized during the construction of the PlayerCharacter."), *ObjectName); \
	}
}
#endif

void APlayerCharacter::ValidateConfigurationAssets()
{
	/** If the configuration properties are not properly serialized, construct a default instance instead. */
	if (!CharacterConfiguration)
	{
		CharacterConfiguration = NewObject<UPlayerCharacterConfiguration>();
		if (GIsEditor && FApp::IsGame())
		{
			UE_LOG(LogPlayerCharacter, Warning, TEXT("No Character Configuration was selected for player character. Using default settings instead."))
		}
	}
}

void APlayerCharacter::HandleLanding(EPlayerLandingType Value)
{
	float StunDuration {0.0f};
	switch(Value)
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

	if (const UWorld* World {GetWorld()})
	{
		if (UPlayerSubsystem* Subsystem {World->GetSubsystem<UPlayerSubsystem>()})
		{
			Subsystem->SetPlayerMovementInputLock(true);
			Subsystem->SetPlayerRotationInputLock(true);
		}
		GetWorld()->GetTimerManager().SetTimer(FallStunTimer, this, &APlayerCharacter::HandleLandingEnd, StunDuration, false);
	}
	GetCharacterMovement()->StopMovementImmediately();
	
	
}

void APlayerCharacter::HandleLandingEnd()
{
	if (PlayerCharacterController)
	{
		if (const UWorld* World {GetWorld()})
		{
			if (UPlayerSubsystem* Subsystem {World->GetSubsystem<UPlayerSubsystem>()})
			{
				Subsystem->SetPlayerMovementInputLock(false);
				Subsystem->SetPlayerRotationInputLock(false);
			}
		}
	}
}

void APlayerCharacter::ApplyConfigurationAssets()
{
	if (CharacterConfiguration)
	{
		CharacterConfiguration->ApplyToPlayerCharacter(this);
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
	return ((Clearance > RequiredClearance || Clearance == -1.f) && CharacterConfiguration->IsJumpingEnabled && !GetMovementComponent()->IsFalling());
}

bool APlayerCharacter::CanCrouch() const
{
	return Super::CanCrouch() && CharacterConfiguration->IsCrouchingEnabled;;
}

bool APlayerCharacter::CanStandUp() const
{
	constexpr float RequiredClearance {100};
	const float Clearance {GetClearanceAbovePawn()};
	return (Clearance > RequiredClearance || Clearance == -1.f && GetMovementComponent()->IsCrouching());
}


void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World {GetWorld()})
	{
		if (UPlayerSubsystem* Subsystem {World->GetSubsystem<UPlayerSubsystem>()})
		{
			Subsystem->UnregisterPlayerCharacter(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void UPlayerCharacterConfiguration::ApplyToPlayerCharacter(const APlayerCharacter* PlayerCharacter)
{
	if (!PlayerCharacter)
	{
		return;
	}
	
	// Set character's movement component properties.
	if (UCharacterMovementComponent* MovementComponent {PlayerCharacter->GetCharacterMovement()})
	{
		MovementComponent->MaxWalkSpeed = WalkSpeed;
		MovementComponent->JumpZVelocity = JumpVelocity;
	}
}

void UPlayerCharacterConfiguration::ApplyToPlayerController(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	if (APlayerCharacterController* CharacterController {Cast<APlayerCharacterController>(PlayerController)})
	{
		CharacterController->CharacterConfiguration = this;
	}
}




















