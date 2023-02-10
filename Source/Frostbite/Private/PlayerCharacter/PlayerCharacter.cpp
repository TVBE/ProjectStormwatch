// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "PlayerAudioController.h"
#include "PlayerCameraController.h"
#include "PlayerCharacterController.h"
#include "PlayerCharacterMovementComponent.h"
#include "PlayerFlashlightController.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/Vector.h"

#include "Frostbite/Frostbite.h"
// Define macros

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Construct Camera.
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(this->RootComponent);
	Camera->SetRelativeLocation(FVector(22.0, 0.0, 75.0));
	Camera->FieldOfView = 90.0;
	Camera->bUsePawnControlRotation = false;

	// Construct FlashlightSpringArm.
	FlashlightSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Flashlight Spring Arm"));
	FlashlightSpringArm->SetupAttachment(this->RootComponent);
	FlashlightSpringArm->SetRelativeLocation(FVector(20.12,2.6,40.9));
	FlashlightSpringArm->SetComponentTickEnabled(false); // We only want the flashlight spring arm to update when the flashlight is enabled.
	FlashlightSpringArm->TargetArmLength = 0.0;
	FlashlightSpringArm->bDoCollisionTest = false;
	FlashlightSpringArm->bUsePawnControlRotation = false;
	FlashlightSpringArm->bEnableCameraLag = false;
	FlashlightSpringArm->bEnableCameraRotationLag = true;
	FlashlightSpringArm->CameraRotationLagSpeed = 8.5;
	
	// Construct Flashlight.
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(FlashlightSpringArm);
	Flashlight->Intensity = 5000.0;
	Flashlight->AttenuationRadius = 3000.0;
	Flashlight->SetVisibility(false); // We don't want the flashlight to be enabled on startup.

	// Construct Audio Components
	BodyAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Body Audio Component"));
	BodyAudioComponent->SetupAttachment(GetMesh(), "spine_04");
	BodyAudioComponent->bAutoActivate = false;
	
	LeftFootAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Left Foot Audio Component"));
	LeftFootAudioComponent->SetupAttachment(GetMesh(), "foot_l_Socket");
	LeftFootAudioComponent->bAutoActivate = false;

	RightFootAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Right Foot Audio Component"));
	RightFootAudioComponent->SetupAttachment(GetMesh(), "foot_r_Socket");
	RightFootAudioComponent->bAutoActivate = false;
	
	// Construct Camera Controller
	CameraController = CreateDefaultSubobject<UPlayerCameraController>(TEXT("Player Camera Controller"));
	CameraController->bWantsInitializeComponent = true;
	CameraController->bEditableWhenInherited = false;
	
	// Construct Flashlight Controller
	FlashlightController = CreateDefaultSubobject<UPlayerFlashlightController>(TEXT("Player Flashlight Controller"));
	FlashlightController->bWantsInitializeComponent = true;
	FlashlightController->bEditableWhenInherited = false;

	// Construct Audio Controller
	AudioController = CreateDefaultSubobject<UPlayerAudioController>(TEXT("Player Audio Controller"));
	AudioController->bWantsInitializeComponent = true;
	AudioController->bEditableWhenInherited = false;

	// Construct VFX Controller
	VFXController = CreateDefaultSubobject<UPlayerVFXController>(TEXT("Player VFX Controller"));
	VFXController->bWantsInitializeComponent = true;
	VFXController->bEditableWhenInherited = false;
}

void APlayerCharacter::PostInitProperties()
{
	Super::PostInitProperties();

#if WITH_EDITOR
	if(!(IsRunningCommandlet() && UE::IsSavingPackage()) && IsRunningGame())
	{
		if(UPlayerCharacterMovementComponent* PlayerCharacterMovementComponent {Cast<UPlayerCharacterMovementComponent>(GetCharacterMovement())})
		{
			PlayerCharacterMovement = PlayerCharacterMovementComponent;
		}
		else
		{
			UE_LOG(LogPlayerCharacter, Error, TEXT("PlayerCharacter does not have PlayerCharacterMovementComponent set as its CharacterMovementComponent."))
		}
		/** Check if this instance of a PlayerCharacter is a blueprint derived class or not. */
		ValidateObject(this, "PlayerCharacter");
		/** Check if all components have been succesfully initialized. */
		ValidateObject(CameraController, "CameraController");
		ValidateObject(FlashlightController, "FlashlightController");
		ValidateObject(AudioController, "AudioController");
		ValidateObject(VFXController, "VFXController");
	}
#endif
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(APlayerCharacterController* PlayerController {Cast<APlayerCharacterController>(GetController())})
	{
		PlayerCharacterController = PlayerController;
	}
	ReceiveControllerChangedDelegate.AddDynamic(this, &APlayerCharacter::HandleControllerChange);
}

// Called when the controller is changed
void APlayerCharacter::HandleControllerChange(APawn* Pawn, AController* OldController, AController* NewController)
{
	if(NewController && NewController != OldController)
	{
		PlayerCharacterController = Cast<APlayerCharacterController>(NewController);
		if(!PlayerCharacterController)
		{
			UE_LOG(LogPlayerCharacter, Warning, TEXT("PlayerCharacter was possesed by a controller that is not a PlayerCharacterController."))
		}
	}
}


// Called every frame
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

void APlayerCharacter::UpdateRotation(float DeltaTime)
{
	const UCharacterMovementComponent* MovementComponent {GetCharacterMovement()};
	if(MovementComponent && ((MovementComponent->IsMovingOnGround() && GetVelocity().X > 1) || MovementComponent->IsFalling()))
	{
		if(GetController())
		{
			SetActorRotation(FRotator(0, GetController()->GetControlRotation().Yaw, 0));
		}
		IsTurningInPlace = false;
	}
	else
	{
		if(IsTurningInPlace)
		{
			AddActorWorldRotation(FRotator(0, CalculateTurnInPlaceRotation(YawDelta, DeltaTime, 4.f, 70.f), 0));
		}
		if(FMath::IsNearlyEqual(YawDelta, 0, 0.5f))
		{
			IsTurningInPlace = false;
		}
		else if(abs(YawDelta) > 30)
		{
			IsTurningInPlace = true;
		}
	}
}

float APlayerCharacter::CalculateTurnInPlaceRotation(const float YawDelta, const float DeltaTime, const float Factor, const float Clamp)
{
	float Rotation {YawDelta * Factor * DeltaTime};
	if(abs(YawDelta) >= Clamp)
	{
		float RotationOvershoot {abs(YawDelta) - Clamp};
		RotationOvershoot = RotationOvershoot * (YawDelta >= 0.0)? 1 : -1;
		Rotation = Rotation + RotationOvershoot; 
	}
	return Rotation;
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacter::SetIsJumping(bool Value)
{
	IsJumping = Value;
}

#if WITH_EDITOR
void APlayerCharacter::ValidateObject(const UObject* Object, const FString ObjectName)
{
	if(!Object)
	{
		UE_LOG(LogPlayerCharacter, Error, TEXT("%s was not properly initialized during the construction of the PlayerCharacter."), *ObjectName); \
	}
}
#endif

















