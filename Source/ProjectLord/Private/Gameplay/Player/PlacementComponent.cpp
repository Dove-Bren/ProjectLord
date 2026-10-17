// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Player/PlacementComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/Map/FogOfWarSubsystem.h"
#include "Gameplay/Player/PlacementActor.h"
#include "Gameplay/Player/LordPlayerController.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/BuildingTypes.h"

UPlacementComponent::UPlacementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPlacementComponent::StartPlacing(const UBuildingType* Type, int Cost, const UPlaceBuildingPurchase* Action)
{
	check(!!Type);
	PlaceType = Type;
	PlaceCost = Cost;
	PlaceAction = Action;

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

TOptional<FPlaceBuildingOperation> UPlacementComponent::AttemptToPlace()
{
	if (!IsPlacing())
	{
		return NullOpt;
	}

	if (!CanPlace())
	{
		return NullOpt;
	}

	const FVector At = ShadowActor->GetActorLocation();

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform Transform(At);
	auto Building = GetWorld()->SpawnActor<ABuilding>(PlaceType->BuildingClass, Transform, Params);

	if (!Building)
	{
		return NullOpt;
	}

	// Capture what happened in an operation
	FPlaceBuildingOperation Operation = {};
	Operation.PlaceCost = PlaceCost;
	Operation.BuildingType = PlaceType;
	Operation.PurchaseAction = PlaceAction;
	Operation.At = At;
	Operation.PlacedBuilding = Building;

	return Operation;
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

	auto Controller = Cast<ALordPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!Controller)
	{
		return false;
	}

	auto FogSubsystem = GetWorld()->GetSubsystem<UFogOfWarSubsystem>();
	if (!ensure(FogSubsystem))
	{
		return false;
	}

	// Check the bounds of the building and see if it ends up in the fog
	FVector Origin;
	FVector Extent;
	ShadowActor->GetActorBounds(true, Origin, Extent);

	for (FVector At : {
		Origin + Extent,
		Origin + (Extent * FVector(-1, 1, 1)),
		Origin + (Extent * FVector(-1, -1, 1)),
		Origin + (Extent * FVector(1, -1, 1)),
	}) {
		if (FogSubsystem->IsInFog(Controller->GetTeam(), At))
		{
			return false;
		}
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
