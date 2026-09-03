// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/PlacementComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/FogOfWarSubsystem.h"
#include "Gameplay/PlacementActor.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/BuildingTypes.h"

UPlacementComponent::UPlacementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPlacementComponent::StartPlacing(const UBuildingType* Type, int Cost)
{
	check(!!Type);
	PlaceType = Type;
	PlaceCost = Cost;

	if (!ShadowActor)
	{
		ShadowActor = SpawnPlacementActor();
	}
	ShadowActor->SetMesh(Type->BuildingPlaceShape);
}

void UPlacementComponent::CancelPlacing()
{
	PlaceType = nullptr;
	PlaceCost = 0;
	if (ensure(ShadowActor))
	{
		ShadowActor->Destroy();
		ShadowActor = nullptr;
	}
}

ABuilding* UPlacementComponent::AttemptToPlace()
{
	if (!IsPlacing())
	{
		return nullptr;
	}

	if (!CanPlace())
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	FTransform Transform(ShadowActor->GetActorLocation());
	auto Building = GetWorld()->SpawnActor<ABuilding>(PlaceType->BuildingClass, Transform, Params);
	return Building;
}

bool UPlacementComponent::CanPlace() const
{
	if (!IsPlacing())
	{
		return false;
	}

	if (!ensure(ShadowActor))
	{
		return false;
	}

	if (ShadowActor->HasOverlap())
	{
		return false;
	}

	auto FogSubsystem = GetWorld()->GetSubsystem<UFogOfWarSubsystem>();
	if (!ensure(FogSubsystem))
	{
		return false;
	}

	if (FogSubsystem->IsInFog(EGameTeam::Player1, ShadowActor->GetActorLocation()))
	{
		return false;
	}

	return true;
}

FVector UPlacementComponent::GetPlaceLocation() const
{
	// Need to do some extra work to find Start and End
	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);

	FHitResult HitResult;
	if (Controller->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldStatic), true, HitResult))
	{
		return HitResult.Location;
	}

	return {};
}

APlacementActor* UPlacementComponent::SpawnPlacementActor()
{
	return GetWorld()->SpawnActor<APlacementActor>(PlacementActorType);
}

void UPlacementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlacementComponent::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
}

void UPlacementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ShadowActor)
	{
		ShadowActor->SetLocation(GetPlaceLocation());
		ShadowActor->UpdatePlaceMaterial(CanPlace());
	}
}
