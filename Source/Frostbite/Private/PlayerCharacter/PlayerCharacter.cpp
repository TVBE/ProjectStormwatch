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
#include "Core/FrostbiteGameMode.h"
#include "Core/PlayerSubsystem.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/Vector.h"
#include "Core/LogCategories.h"


/** The PlayerCharacter's initialization follows these stages:
 *	Constructor: Creates the actor and sets its default properties. We cannot access default property values at this time.
 *	PostInitProperties(): Called after construction to perform additional initialization that requires access to default property values.
 *	OnConstruction(): Called after all default property values have been fully initialized, but before any of the components are initialized.
 *	PostInitializeComponents(): Called after initializing the components, which allows them to register with other systems and set up data structures.
 *	BeginPlay(): Called when the actor is ready to be used in the game world.
 */

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
	
	// Construct Flashlight.
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(FlashlightSpringArm);
	Flashlight->SetVisibility(false); // We don't want the flashlight to be enabled on startup.
	
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
	CameraController->bEditableWhenInherited = false;
	
	// Construct Flashlight Controller
	FlashlightController = CreateDefaultSubobject<UPlayerFlashlightController>(TEXT("Flashlight Controller"));
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
		AudioController = CreateDefaultSubobject<UPlayerAudioController>(TEXT("Audio Controller")); 
	}
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
		VfxController = CreateDefaultSubobject<UPlayerVfxController>(TEXT("VFX Controller"));
	}
	VfxController->bEditableWhenInherited = false;
}

// Called after the constructor but before InitializeComponents.
void APlayerCharacter::PostInitProperties()
{
	ValidateConfigurationAssets();
	
	if(UPlayerCharacterMovementComponent* PlayerCharacterMovementComponent {Cast<UPlayerCharacterMovementComponent>(GetCharacterMovement())})
	{
		PlayerCharacterMovement = PlayerCharacterMovementComponent;
	}
	else if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, FLT_MAX, FColor::Red, "PlayerCharacter failed to initialize PlayerCharacterMovementComponent.");
	}

	// Set components to call their virtual InitializeComponent functions.
	CameraController->bWantsInitializeComponent = true;
	FlashlightController->bWantsInitializeComponent = true;
	AudioController->bWantsInitializeComponent = true;
	VfxController->bWantsInitializeComponent = true;
	
	Super::PostInitProperties();
}

// Called after all default property values have been fully initialized, but before any of the components are initialized.
void APlayerCharacter::OnConstruction(const FTransform& Transform)
{
	// Registers this player character to the player character subsystem.
	if(const UWorld* World {GetWorld()})
	{
		if(UPlayerSubsystem* PlayerSubsystem {World->GetSubsystem<UPlayerSubsystem>()})
		{
			PlayerSubsystem->RegisterPlayerCharacter(this);
		}
	}
	
	Super::OnConstruction(Transform);
}

// Called after InitializeComponents.
void APlayerCharacter::PostInitializeComponents()
{
	ApplyConfigurationAssets();

	// Subscribe to the OnLanding event of the player character movement component.
	if(PlayerCharacterMovement)
	{
		PlayerCharacterMovement->OnLanding.AddDynamic(this, &APlayerCharacter::HandleLanding);
	}
	
	Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Notify the GameMode that the character has Begun Play.
	if(GetWorld() && GetWorld()->GetAuthGameMode())
	{
		if(AFrostbiteGameMode* GameMode {Cast<AFrostbiteGameMode>(GetWorld()->GetAuthGameMode())})
		{
			GameMode->NotifyPlayerCharacterBeginPlay(this);
		}
	}

#if WITH_EDITOR
		/** Check if all components have been succesfully initialized. */
		ValidateObject(CameraController, "CameraController");
		ValidateObject(FlashlightController, "FlashlightController");
		ValidateObject(AudioController, "AudioController");
		ValidateObject(VfxController, "VfxController");
		ValidateObject(BodyAudioComponent, "BodyAudioComponent");
		ValidateObject(LeftFootAudioComponent, "LeftFootAudioComponent");
		ValidateObject(RightFootAudioComponent, "RightFootAudioComponent");
#endif
}

// Called when the controller is changed
void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if(NewController)
	{
		PlayerCharacterController = Cast<APlayerCharacterController>(NewController);
		if(PlayerCharacterController)
		{
			// Registers the new controller to the player character subsystem.
			if(const UWorld* World {GetWorld()})
			{
				if(UPlayerSubsystem* PlayerSubsystem {World->GetSubsystem<UPlayerSubsystem>()})
				{
					PlayerSubsystem->RegisterPlayerController(Cast<APlayerCharacterController>(PlayerCharacterController));
				}
			}	
		}
		CameraConfiguration->ApplyToPlayerController(Cast<APlayerController>(NewController));
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
		constexpr float YawDeltaThreshold {30.0f};
		constexpr float YawDeltaClamp {45.0f};
		
		if(IsTurningInPlace)
		{
			AddActorWorldRotation(FRotator(0, CalculateTurnInPlaceRotation(YawDelta, DeltaTime, 4.f, YawDeltaClamp), 0));
		}
		if(FMath::IsNearlyEqual(YawDelta, 0, 0.5f))
		{
			IsTurningInPlace = false;
		}
		else if(abs(YawDelta) > YawDeltaThreshold)
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
		RotationOvershoot = (YawDelta >= 0.0) ? RotationOvershoot : -RotationOvershoot;
		Rotation += RotationOvershoot;
	}
	return Rotation;
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

void APlayerCharacter::ValidateConfigurationAssets()
{
	/** If the configuration properties are not properly serialized, construct a default instance instead. */
	if(!CharacterConfiguration)
	{
		CharacterConfiguration = NewObject<UPlayerCharacterConfiguration>();
		if(GIsEditor && FApp::IsGame())
		{
			UE_LOG(LogPlayerCharacter, Warning, TEXT("No Character Configuration was selected for player character. Using default settings instead."))
		}
	}
	if(!CameraConfiguration)
	{
		CameraConfiguration = NewObject<UPlayerCameraConfiguration>();
		if(GIsEditor && FApp::IsGame())
		{
			UE_LOG(LogPlayerCharacter, Warning, TEXT("No Camera Configuration was selected for player character. Using default settings instead."))
		}
	}
	if(!FlashlightConfiguration)
	{
		FlashlightConfiguration = NewObject<UPlayerFlashlightConfiguration>();
		if(GIsEditor && FApp::IsGame())
		{
			UE_LOG(LogPlayerCharacter, Warning, TEXT("No Flashlight Configuration was selected for player character. Using default settings instead."))
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

	if(const UWorld* World {GetWorld()})
	{
		if(UPlayerSubsystem* Subsystem {World->GetSubsystem<UPlayerSubsystem>()})
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
	if(PlayerCharacterController)
	{
		if(const UWorld* World {GetWorld()})
		{
			if(UPlayerSubsystem* Subsystem {World->GetSubsystem<UPlayerSubsystem>()})
			{
				Subsystem->SetPlayerMovementInputLock(false);
				Subsystem->SetPlayerRotationInputLock(false);
			}
		}
	}
}

void APlayerCharacter::ApplyConfigurationAssets()
{
	if(CharacterConfiguration)
	{
		CharacterConfiguration->ApplyToPlayerCharacter(this);
	}
	if(CameraConfiguration)
	{
		CameraConfiguration->ApplyToPlayerCharacter(this);
	}
	if(FlashlightConfiguration)
	{
		FlashlightConfiguration->ApplyToPlayerCharacter(this);
	}
}



















