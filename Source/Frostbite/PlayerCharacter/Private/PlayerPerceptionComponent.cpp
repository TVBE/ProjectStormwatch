// Copyright Notice

#include "PlayerPerceptionComponent.h"

#include "FrostbiteFunctionLibrary.h"
#include "Nightstalker.h"
#include "NightstalkerAIFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY_CLASS(UPlayerPerceptionComponent, LogPlayerPerception);

UPlayerPerceptionComponent::UPlayerPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
}

void UPlayerPerceptionComponent::OnRegister()
{
	Super::OnRegister();

	if (const APlayerCharacter* PlayerCharacter {Cast<APlayerCharacter>(GetOwner())})
	{
		Camera = PlayerCharacter->GetCamera();
	}
	else
	{
		UE_LOG(LogPlayerPerception, Warning, TEXT("PlayerPerceptionComponent was added to another class than APlayerCharacter."))
	}
}


void UPlayerPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(PerceptionTimerHandle, this, &UPlayerPerceptionComponent::UpdatePerception, PerceptionUpdateInterval, true);
}

void UPlayerPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsNightstalkerVisible)
	{
		NightstalkerVisibilityDuration += DeltaTime;
		if (NightstalkerVisibilityDuration >= NightstalkerDetectionThreshold)
		{
			OnNightstalkerPerceptionChanged.Broadcast(true);
			IsNightstalkerDetected = true;
		}
	}
}

float UPlayerPerceptionComponent::GetViewAngleToTarget(const FVector& Target)
{
	if (Camera)
	{
		const FVector DirectionToTarget {(Target - Camera->GetComponentLocation()).GetSafeNormal()};
		
		const float DotProduct {static_cast<float>(FVector::DotProduct(Camera->GetForwardVector(), DirectionToTarget))};
		
		const float ViewAngle {FMath::Acos(DotProduct) * (180.0f / PI)};
		
		return ViewAngle;
	}
	else
	{
		UE_LOG(LogPlayerPerception, Warning, TEXT("GetViewAngleToTarget: No UCameraComponent available."));
		return 0.0f;
	}
}

void UPlayerPerceptionComponent::UpdatePerception_Implementation()
{
	if (const ANightstalker* Nightstalker {UFrostbiteFunctionLibrary::GetFrostbiteNightstalker(this)})
	{
		const FVector PlayerLocation {GetOwner()->GetActorLocation()};
		const FVector NightstalkerLocation {Nightstalker->GetActorLocation()};
		
		if (const float ViewAngleToNightstalker {GetViewAngleToTarget(NightstalkerLocation)}; ViewAngleToNightstalker <= 80)
		{
			UE_LOG(LogPlayerPerception, VeryVerbose, TEXT("UpdatePerception: View angle to Nightstalker: %f"), ViewAngleToNightstalker);
			
			if (!UNightstalkerAIFunctionLibrary::IsOccluded(this, PlayerLocation, NightstalkerLocation))
			{
				UE_LOG(LogPlayerPerception, VeryVerbose, TEXT("UpdatePerception: Nightstalker is not occluded."));
				
				IsNightstalkerVisible = true;
				return;
			}
		}
		
		IsNightstalkerVisible = false;
		NightstalkerVisibilityDuration = 0.0f;

		if (IsNightstalkerDetected)
		{
			IsNightstalkerDetected = false;
			OnNightstalkerPerceptionChanged.Broadcast(false);
		}
		
	}
}


