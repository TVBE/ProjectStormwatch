// Copyright (c) 2022-present Barrelhouse
// Written by Tim Verberne
// This source code is part of the project Frostbite

#include "HeatPoint.h"

#include "NightstalkerDirector.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

AHeatPoint::AHeatPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
	RootComponent = SphereComponent;
	
#if WITH_EDITORONLY_DATA
	ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	DebugSphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Debug Sphere Mesh"));
	if (SphereMesh.Succeeded())
	{
		DebugSphereMesh->SetStaticMesh(SphereMesh.Object);
		DebugSphereMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DebugSphereMesh->SetVisibility(false);
		DebugSphereMesh->SetupAttachment(RootComponent);

		ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("Material'/Game/Game/Tests/M_HeatPointDebugMaterial_01.M_HeatPointDebugMaterial_01'"));
		if (MaterialFinder.Succeeded())
		{
		DebugSphereMesh->SetMaterial(0, DebugMaterial);
			UE_LOG(LogTemp, Warning, TEXT("Couldn't find material!"))
		}
	}
#endif
}

void AHeatPoint::InitializeHeatPoint(const int RadiusValue, const int LifeTimeValue, const float HeatValue)
{
	Radius = RadiusValue;
	Lifetime = LifeTimeValue;
	Heat = HeatValue;

	SetRadius(Radius);
}

void AHeatPoint::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITORONLY_DATA
	if (DebugSphereMesh)
	{
		UMaterialInterface* BaseMaterial {DebugSphereMesh->GetMaterial(0)};
		if (BaseMaterial)
		{
			DebugMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			DebugSphereMesh->SetMaterial(0, DebugMaterial);
		}
	}
	
	if (IsDebugVisEnabled)
	{
		SetDebugVisEnabled(true);
	}
#endif
}

void AHeatPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		if (UNightstalkerDirector* DirectorSubsystem = World->GetSubsystem<UNightstalkerDirector>())
		{
			DirectorSubsystem->UnregisterHeatPoint(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void AHeatPoint::Update()
{
}

void AHeatPoint::SetRadius(const int NewRadius)
{
	if (SphereComponent)
	{
		SphereComponent->SetSphereRadius(Radius);

#if WITH_EDITORONLY_DATA
		if (DebugSphereMesh)
		{
			DebugSphereMesh->SetWorldScale3D(FVector(Radius / 50.0f));
		}
#endif
	}
}

void AHeatPoint::SetHeat(const float NewHeat)
{
	Heat = FMath::Clamp(NewHeat, 0.0f, 100.0f);

#if WITH_EDITORONLY_DATA
	if (DebugMaterial)
	{
		DebugMaterial->SetScalarParameterValue(FName("Heat"), NewHeat);
	}
#endif
}

void AHeatPoint::AddLifeTime(const int Time)
{
	Lifetime += Time;
}

void AHeatPoint::AddHeat(const float AddedHeat)
{
	const float HeatTotal {Heat + AddedHeat};
	SetHeat(HeatTotal);
}

float AHeatPoint::DecrementLifeTime(const float Value)
{
	Lifetime = FMath::Max(0.0f, Lifetime - Value);
	return Lifetime;
}

#if WITH_EDITORONLY_DATA
void AHeatPoint::SetDebugVisEnabled(bool IsEnabled)
{
	if (DebugSphereMesh)
	{
		DebugSphereMesh->SetVisibility(IsEnabled);
		IsDebugVisEnabled = true;
	}
}
#endif


