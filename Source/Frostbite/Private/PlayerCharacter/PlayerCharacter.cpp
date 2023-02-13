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
#include "NiagaraComponent.h"
#include "MetasoundSource.h"
#include "PlayerVfxController.h"
#include "Core/PlayerSubsystem.h"
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
	Flashlight->SetVisibility(false); // We don't want the flashlight to be enabled on startup.
	
	// Default settings that generally work well for the flashlight.
	Flashlight->Intensity = 0.75;
	Flashlight->AttenuationRadius = 4000.0;
	Flashlight->LightColor = FColor(233,255,254,255);
	Flashlight->OuterConeAngle = 34.0f;
	Flashlight->InnerConeAngle = 22.0f;
	Flashlight->bAffectsWorld = true;
	Flashlight->CastShadows = false;
	Flashlight->bUseInverseSquaredFalloff = false;
	Flashlight->LightFalloffExponent = 4.0f;
	Flashlight->VolumetricScatteringIntensity = 0.0f;
	
	// Construct Audio Components
	BodyAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Body Audio Component"), true);
	BodyAudioComponent->SetupAttachment(GetMesh(), "spine_04");
	BodyAudioComponent->bAutoActivate = false;
	BodyAudioComponent->bEditableWhenInherited = false;
	
	static ConstructorHelpers::FObjectFinder<UMetaSoundSource> MainSourceMetasound(TEXT("/Script/MetasoundEngine.MetaSoundSource'/Game/Game/Audio/Sources/Player/Main/MSS_Player_Main.MSS_Player_Main'"));
	if(MainSourceMetasound.Succeeded())
	{
		BodyAudioComponent->SetSound(MainSourceMetasound.Object);
	}
	
	LeftFootAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Left Foot Audio Component"), true);
	LeftFootAudioComponent->SetupAttachment(GetMesh(), "foot_l_Socket");
	LeftFootAudioComponent->bAutoActivate = false;
	LeftFootAudioComponent->bEditableWhenInherited = false;
	
	RightFootAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Right Foot Audio Component"), true);
	RightFootAudioComponent->SetupAttachment(GetMesh(), "foot_r_Socket");
	RightFootAudioComponent->bAutoActivate = false;
	RightFootAudioComponent->bEditableWhenInherited = false;

	static ConstructorHelpers::FObjectFinder<UMetaSoundSource> FootstepSourceMetasound(TEXT("/Script/MetasoundEngine.MetaSoundSource'/Game/Game/Audio/Sources/Player/Footsteps/MSS_Player_Footstep.MSS_Player_Footstep'"));
	if(FootstepSourceMetasound.Succeeded())
	{
		LeftFootAudioComponent->SetSound(FootstepSourceMetasound.Object);
		RightFootAudioComponent->SetSound(FootstepSourceMetasound.Object);
	}

	// Construct Particle System Components
	LeftFootParticleEmitter = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Left Foot Particle Emitter"), true);
	LeftFootParticleEmitter->SetupAttachment(GetMesh(), "foot_l_Socket");
	LeftFootParticleEmitter->bAutoActivate = false;
	LeftFootParticleEmitter->bEditableWhenInherited = false;

	RightFootParticleEmitter = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Right Foot Particle Emitter"),true);
	RightFootParticleEmitter->SetupAttachment(GetMesh(), "foot_r_Socket");
	RightFootParticleEmitter->bAutoActivate = false;
	RightFootParticleEmitter->bEditableWhenInherited = false;
	
	// Construct Camera Controller
	CameraController = CreateDefaultSubobject<UPlayerCameraController>(TEXT("Camera Controller"));
	CameraController->bWantsInitializeComponent = true;
	CameraController->bEditableWhenInherited = false;
	
	// Construct Flashlight Controller
	FlashlightController = CreateDefaultSubobject<UPlayerFlashlightController>(TEXT("Flashlight Controller"));
	FlashlightController->bWantsInitializeComponent = true;
	FlashlightController->bEditableWhenInherited = false;

	// Construct Audio Controller, we want to use the Blueprint derived class for this so that designers can easily script behavior for the audio controller.
	static ConstructorHelpers::FClassFinder<UPlayerAudioController> AudioControllerClass(TEXT("/Script/Engine.Blueprint'/Game/Game/Actors/PlayerCharacter/Blueprints/Components/BPC_PlayerAudioController.BPC_PlayerAudioController_C'"));
	if(AudioControllerClass.Succeeded())
	{
		AudioController = Cast<UPlayerAudioController>(CreateDefaultSubobject(TEXT("Audio Controller"), AudioControllerClass.Class, AudioControllerClass.Class, true, true ));
	}
	else
	{
		// Construct the base class if the Blueprint derived class cannot be found.
		AudioController = CreateDefaultSubobject<UPlayerAudioController>(TEXT("Audio Controller Fallback")); 
	}
	AudioController->bWantsInitializeComponent = true;
	AudioController->bEditableWhenInherited = false;
	
	// Construct VFX Controller, we want to use the Blueprint derived class for this so that designers can easily script behavior for the VFX controller.
	static ConstructorHelpers::FClassFinder<UPlayerVfxController> VfxControllerClass(TEXT("/Script/Engine.Blueprint'/Game/Game/Actors/PlayerCharacter/Blueprints/Components/BPC_PlayerVfxController.BPC_PlayerVfxController_C'"));
	if(VfxControllerClass.Succeeded())
	{
		VfxController = Cast<UPlayerVfxController>(CreateDefaultSubobject(TEXT("VFX Controller"), VfxControllerClass.Class, VfxControllerClass.Class, true, true ));
	}
	else
	{
		// Construct the base class if the Blueprint derived class cannot be found.
		VfxController = CreateDefaultSubobject<UPlayerVfxController>(TEXT("VFX Controller Fallback"));
	}
	VfxController->bWantsInitializeComponent = true;
	VfxController->bEditableWhenInherited = false;
}

void APlayerCharacter::PostInitProperties()
{
	Super::PostInitProperties();

	/** If the configuration properties are not properly serialized, construct a default data asset object instead. */
	if(!CharacterConfiguration)
	{
		CharacterConfiguration = NewObject<UPlayerCharacterConfiguration>();
		//UE_LOG(LogPlayerCharacter, Warning, TEXT("No PlayerCharacterConfiguration asset was specified for PlayerCharacter, using default settings instead."))
	}
	if(!CameraConfiguration)
	{
		CameraConfiguration = NewObject<UPlayerCameraConfiguration>();
		//UE_LOG(LogPlayerCharacter, Warning, TEXT("No PlayerCameraConfiguration asset was specified for PlayerCharacter, using default settings instead."))
	}
	if(UPlayerCharacterMovementComponent* PlayerCharacterMovementComponent {Cast<UPlayerCharacterMovementComponent>(GetCharacterMovement())})
	{
		PlayerCharacterMovement = PlayerCharacterMovementComponent;
	}
	else if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, FLT_MAX, FColor::Red, "PlayerCharacter failed to initialize PlayerCharacterMovementComponent.");
	}

#if WITH_EDITOR
	if(!(IsRunningCommandlet() && UE::IsSavingPackage()) && IsRunningGame())
	{
		/** Check if all components have been succesfully initialized. */
		ValidateObject(CameraController, "CameraController");
		ValidateObject(FlashlightController, "FlashlightController");
		ValidateObject(AudioController, "AudioController");
		ValidateObject(VfxController, "VfxController");
		ValidateObject(BodyAudioComponent, "BodyAudioComponent");
		ValidateObject(LeftFootAudioComponent, "LeftFootAudioComponent");
		ValidateObject(RightFootAudioComponent, "RightFootAudioComponent");
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
	if(CharacterConfiguration && PlayerCharacterController)
	{
		CharacterConfiguration->ApplyToPlayerCharacterInstance(this);
	}

	// Registers this player character to the player character subsystem.
	if(const UWorld* World {GetWorld()})
	{
		if(UPlayerSubsystem* PlayerSubsystem {World->GetSubsystem<UPlayerSubsystem>()})
		{
			PlayerSubsystem->RegisterPlayerCharacter(this);
		}
	}
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

















