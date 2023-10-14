// Copyright Notice

#include "BHPlayerPerceptionComponent.h"
#include "StormwatchFunctionLibrary.h"
#include "Nightstalker.h"
#include "NightstalkerAIFunctionLibrary.h"
#include "BHPlayerCharacter.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY_CLASS(UBHPlayerPerceptionComponent, LogPlayerPerception);

UBHPlayerPerceptionComponent::UBHPlayerPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UBHPlayerPerceptionComponent::OnRegister()
{
	Super::OnRegister();

	if (const ABHPlayerCharacter* PlayerCharacter = Cast<ABHPlayerCharacter>(GetOwner());)
	{
		Camera = PlayerCharacter->GetCamera();
	}
	else
	{
		UE_LOG(LogPlayerPerception, Warning, TEXT("PlayerPerceptionComponent was added to another class than ABHPlayerCharacter."))
	}
}


void UBHPlayerPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(PerceptionTimerHandle, this, &UBHPlayerPerceptionComponent::UpdatePerception, PerceptionUpdateInterval, true);
}

void UBHPlayerPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bNightstalkerVisible)
	{
		NightstalkerVisibilityDuration += DeltaTime;
		if (NightstalkerVisibilityDuration >= NightstalkerDetectionThreshold)
		{
			OnNightstalkerPerceptionChanged.Broadcast(true);
			bNightstalkerDetected = true;
		}
	}
}

float UBHPlayerPerceptionComponent::GetViewAngleToTarget(const FVector& Target)
{
	if (Camera)
	{
		const FVector DirectionToTarget = (Target - Camera->GetComponentLocation()).GetSafeNormal();
		
		const float DotProduct = static_cast<float>(FVector::DotProduct(Camera->GetForwardVector(), DirectionToTarget));
		
		const float ViewAngle = FMath::Acos(DotProduct) * (180.0f / PI);
		
		return ViewAngle;
	}
	else
	{
		UE_LOG(LogPlayerPerception, Warning, TEXT("GetViewAngleToTarget: No UCameraComponent available."));
		return 0.0f;
	}
}

void UBHPlayerPerceptionComponent::UpdatePerception_Implementation()
{
	if (const ANightstalker* Nightstalker = UStormwatchFunctionLibrary::GetStormwatchNightstalker(this);)
	{
		const FVector PlayerLocation = GetOwner()->GetActorLocation();
		const FVector NightstalkerLocation = Nightstalker->GetActorLocation();
		
		if (const float ViewAngleToNightstalker = GetViewAngleToTarget(NightstalkerLocation); ViewAngleToNightstalker <= 80)
		{
			UE_LOG(LogPlayerPerception, VeryVerbose, TEXT("UpdatePerception: View angle to Nightstalker: %f"), ViewAngleToNightstalker);
			
			if (!UNightstalkerAIFunctionLibrary::IsOccluded(this, PlayerLocation, NightstalkerLocation))
			{
				UE_LOG(LogPlayerPerception, VeryVerbose, TEXT("UpdatePerception: Nightstalker is not occluded."));
				
				bNightstalkerVisible = true;
				return;
			}
		}
		
		bNightstalkerVisible = false;
		NightstalkerVisibilityDuration = 0.0f;

		if (bNightstalkerDetected)
		{
			bNightstalkerDetected = false;
			OnNightstalkerPerceptionChanged.Broadcast(false);
		}
		
	}
}


