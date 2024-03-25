// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHPlayerCharacter.h"

#include "BHPlayerBodyComponent.h"
#include "BHPlayerCameraComponent.h"
#include "BHPlayerCameraSocketComponent.h"
#include "BHPlayerSkeletalMeshComponent.h"
#include "BHPlayerController.h"
#include "BHPlayerDragComponent.h"
#include "BHPlayerFootComponent.h"
#include "BHPlayerGrabComponent.h"
#include "BHPlayerInteractionComponent.h"
#include "BHPlayerInventoryComponent.h"
#include "BHCharacterMovementComponent.h"
#include "BHPlayerUseComponent.h"
#include "BHStormwatchGameMode.h"
#include "..\..\..\Core\Public\BHStormwatchWorldSubystem.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Math/Vector.h"

ABHPlayerCharacter::ABHPlayerCharacter(const FObjectInitializer& ObjectInitializer)
: Super(
	ObjectInitializer
	.SetDefaultSubobjectClass<UBHPlayerSkeletalMeshComponent>(MeshComponentName)
)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	CameraSocketComponent = CreateDefaultSubobject<UBHPlayerCameraSocketComponent>(TEXT("CameraSocketComponent"));
	if(CameraSocketComponent)
	{
		CameraSocketComponent->SetRelativeLocation(FVector(22.0, 0.0, 75.0));
		CameraSocketComponent->bEditableWhenInherited = true;
		CameraSocketComponent->PrimaryComponentTick.TickGroup = TG_DuringPhysics;
	}

	Camera = CreateDefaultSubobject<UBHPlayerCameraComponent>(TEXT("Camera"));
	if (Camera)
	{
		Camera->SetupAttachment(this->CameraSocketComponent);
		Camera->FieldOfView = 90.0;
		Camera->bUsePawnControlRotation = false;
		Camera->PrimaryComponentTick.TickGroup = TG_PostPhysics;
	}
	
	UseComponent = CreateDefaultSubobject<UBHPlayerUseComponent>(TEXT("UseComponent"));
	if (UseComponent)
	{
		UseComponent->bEditableWhenInherited = true;
		UseComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	}

	GrabComponent = CreateDefaultSubobject<UBHPlayerGrabComponent>(TEXT("GrabComponent"));
	if (GrabComponent)
	{
		GrabComponent->bEditableWhenInherited = true;
		GrabComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	}

	DragComponent = CreateDefaultSubobject<UBHPlayerDragComponent>(TEXT("DragComponent"));
	if (DragComponent)
	{
		DragComponent->bEditableWhenInherited = true;
		DragComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	}

	InventoryComponent = CreateDefaultSubobject<UBHPlayerInventoryComponent>(TEXT("InventoryComponent"));
	if (InventoryComponent)
	{
		InventoryComponent->bEditableWhenInherited = true;
	}

	BodyCollision = CreateDefaultSubobject<UBHPlayerBodyComponent>(TEXT("BodyCollision"));
	if (BodyCollision)
	{
		BodyCollision->SetupAttachment(GetMesh());
		BodyCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
		BodyCollision->SetCapsuleHalfHeight(72.0f);
		BodyCollision->SetCapsuleRadius(34.0f);
		BodyCollision->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	}

	LeftFootComponent = CreateDefaultSubobject<UBHPlayerFootComponent>(TEXT("LeftFootComponent"));
	if (LeftFootComponent)
	{
		LeftFootComponent->SetupAttachment(GetMesh(), FName("foot_l_socket"));
		LeftFootComponent->bEditableWhenInherited = true;
		LeftFootComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	}

	RightFootComponent = CreateDefaultSubobject<UBHPlayerFootComponent>(TEXT("RightFootComponent"));
	if (RightFootComponent)
	{
		RightFootComponent->SetupAttachment(GetMesh(), FName("foot_r_socket"));
		RightFootComponent->bEditableWhenInherited = true;
		RightFootComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	}
}

void ABHPlayerCharacter::PostLoad()
{
	Super::PostLoad();
	
	if (ensureAlwaysMsgf(GetCharacterMovement()->IsA(UBHCharacterMovementComponent::StaticClass()),
		TEXT("BHPlayerCharacter::PostLoad: CharacterMovement is not of class PlayerCharacterMovementComponent.")))
	{
		PlayerMovementComponent = Cast<UBHCharacterMovementComponent>(GetCharacterMovement());
	}
	if (ensureAlwaysMsgf(GetMesh()->IsA(UBHPlayerSkeletalMeshComponent::StaticClass()),
		TEXT("BHPlayerCharacter::PostLoad: Mesh is not of class PlayerCharacterMovementComponent.")))
	{
		PlayerMesh = Cast<UBHPlayerSkeletalMeshComponent>(GetMesh());
	}
}

void ABHPlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!IsTemplate())
	{
		UBHStormwatchWorldSubsystem* StormwatchWorldSubsystem =
			GetWorld() ? GetWorld()->GetSubsystem<UBHStormwatchWorldSubsystem>() : nullptr;
		check(StormwatchWorldSubsystem);
		StormwatchWorldSubsystem->Register(this);
	}
}

void ABHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	ABHStormwatchGameMode* GameMode = Cast<ABHStormwatchGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode);
	GameMode->NotifyPlayerCharacterBeginPlay(this);
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
		bTurningInPlace = false;
	}
	else
	{
		constexpr float YawDeltaThreshold = 30.0f;

		if (bTurningInPlace)
		{
			AddActorWorldRotation(FRotator(0, CalculateTurnInPlaceRotation(YawDelta, DeltaTime, 4.f, 45.0f), 0));
		}
		if (FMath::IsNearlyEqual(YawDelta, 0, 0.5f))
		{
			bTurningInPlace = false;
		}
		else if (abs(YawDelta) > YawDeltaThreshold)
		{
			bTurningInPlace = true;
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

UBHPlayerCameraSocketComponent* ABHPlayerCharacter::GetCameraSocketComponent() const
{
	check(CameraSocketComponent);
	return CameraSocketComponent;
}

UBHPlayerSkeletalMeshComponent* ABHPlayerCharacter::GetPlayerMesh() const
{
	check(PlayerMesh)
	return PlayerMesh;
}

UBHCharacterMovementComponent* ABHPlayerCharacter::GetPlayerMovementComponent() const
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

bool ABHPlayerCharacter::IsInteracting() const
{
	if(!ensureAlways(GrabComponent && DragComponent && UseComponent))
	{
		return false;
	}
	return GrabComponent->IsHoldingObject() || DragComponent->IsHoldingObject() || UseComponent->IsUsingObject();
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
		PlayerController->SetMovementInputLock(true);
		PlayerController->SetRotationInputLock(true);
	}
	GetWorld()->GetTimerManager().SetTimer(FallStunTimer, this, &ABHPlayerCharacter::HandleLandingEnd, StunDuration, false);
	
	GetCharacterMovement()->StopMovementImmediately();
}

void ABHPlayerCharacter::HandleLandingEnd()
{
	if (ABHPlayerController* PlayerController = Cast<ABHPlayerController>(GetController()))
	{
		PlayerController->SetMovementInputLock(false);
		PlayerController->SetRotationInputLock(false);
	}
}
