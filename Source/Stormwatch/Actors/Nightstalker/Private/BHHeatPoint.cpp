// Copyright (c) 2022-present Barrelhouse. All rights reserved.

#include "BHHeatPoint.h"

#include "BHNightstalkerDirector.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ABHHeatPoint::ABHHeatPoint()
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

		ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("/Script/Engine.Material'/Game/Game/Tests/M_HeatPointDebugMaterial_01.M_HeatPointDebugMaterial_01'"));
		if (MaterialFinder.Succeeded())
		{
		DebugSphereMesh->SetMaterial(0, MaterialFinder.Object);
		}
	}
#endif
}

void ABHHeatPoint::InitializeHeatPoint(const int RadiusValue, const int ExpirationValue, const float HeatValue)
{
	ExpirationTime = ExpirationValue;
	SetHeat(HeatValue);
	SetRadius(RadiusValue);
}

void ABHHeatPoint::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITORONLY_DATA
	if (DebugSphereMesh)
	{
		if (UMaterialInterface* BaseMaterial = DebugSphereMesh->GetMaterial(0))
		{
			DebugMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
			DebugSphereMesh->SetMaterial(0, DebugMaterial);
		}
	}
	
	if (bDebugVisEnabled)
	{
		SetDebugVisEnabled(true);
	}
#endif
}

void ABHHeatPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const UWorld* World = GetWorld())
	{
		if (UBHNightstalkerDirector* DirectorSubsystem = World->GetSubsystem<UBHNightstalkerDirector>())
		{
			DirectorSubsystem->UnregisterHeatPoint(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void ABHHeatPoint::UpdateLifeTime(const int DeltaTime)
{
	Lifetime += DeltaTime;
	ExpirationTime = FMath::Max(0.0f, ExpirationTime - DeltaTime);
}

void ABHHeatPoint::SetRadius(const int NewRadius)
{
	Radius = NewRadius;
	
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

void ABHHeatPoint::SetHeat(const float NewHeat)
{
	Heat = FMath::Clamp(NewHeat, 0.0f, 100.0f);

#if WITH_EDITORONLY_DATA
	if (DebugMaterial)
	{
		DebugMaterial->SetScalarParameterValue(FName("Heat"), Heat);
	}
#endif
}

void ABHHeatPoint::AddHeat(const float AddedHeat)
{
	const float HeatTotal = Heat + AddedHeat;
	SetHeat(HeatTotal);
}

void ABHHeatPoint::DetractHeat(const float HeatToDeduct)
{
	const float HeatTotal = Heat - HeatToDeduct;
	SetHeat(HeatTotal);
}

#if WITH_EDITORONLY_DATA
void ABHHeatPoint::SetDebugVisEnabled(bool IsEnabled)
{
	if (DebugSphereMesh)
	{
		DebugSphereMesh->SetVisibility(IsEnabled);
		bDebugVisEnabled = true;
	}
}
#endif


