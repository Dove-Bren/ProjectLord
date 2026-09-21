// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/Castle.h"

#include "Gameplay/LordGameState.h"
#include "Gameplay/PlacementActor.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/BuildingTypes.h"
#include "Gameplay/Units/HeroBase.h"
#include "Gameplay/Units/UnitTypes.h"

void ACastle::BeginPlay()
{
	Super::BeginPlay();

	NextSpawnTime = GetGameTimeSinceCreation() + .5f;
	bStartActivated = true;
}

void ACastle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	float Time = GetGameTimeSinceCreation();
	if (Time > NextSpawnTime)
	{
		NextSpawnTime = Time + 10 + FMath::RandRange(5.0f, 20.0f);
		AttemptSpawnRound();
	}

	if (int Gold = CollectBuildingGold())
	{
		// Deposit directly into team gold
		if (auto TeamState = GetTeamState())
		{
			TeamState->AddGold(Gold);
		}
	}
}

bool ACastle::WantsTaxCollection() const
{
	return false;
}

AGameTeamState* ACastle::GetTeamState() const
{
	if (auto State = GetWorld()->GetGameState<ALordGameState>())
	{
		return State->GetTeam(GetTeam());
	}
	return nullptr;
}

int ACastle::GetHouseCount() const
{
	int Count = 0;
	auto TeamState = GetTeamState();
	if (ensure(TeamState))
	{
		// just here to make this visible
		ensure(!HouseTypes.IsEmpty());
		for (auto HouseType : HouseTypes)
		{
			Count += TeamState->GetTeamBuildingsOfType(HouseType).Num();
		}
	}

	return Count;
}

int ACastle::GetHeroCount() const
{
	int Count = 0;
	auto TeamState = GetTeamState();
	if (ensure(TeamState))
	{
		Count += TeamState->GetTeamUnitsOfClass(AHeroBase::StaticClass()).Num();
	}

	return Count;
}

int ACastle::GetBuildingCount() const
{
	auto TeamState = GetTeamState();
	if (ensure(TeamState))
	{
		return TeamState->GetBuildings().Num();
	}
	//else
	return 0;
}

int ACastle::GetDesiredHouseCount() const
{
	// Just seems to always be 5?
	return 5;
}

int ACastle::GetDesiredTaxCollectorCount() const
{
	// Idk why but this always seems to be 6
	return 6;
}

int ACastle::GetDesiredRoyalGuardCount() const
{
	// This is either always 6, or 3+level (4, 5, 6)
	return 3 + GetBuildingLevel();
}

void ACastle::AttemptSpawnRound()
{

	if (GetHouseCount() < GetDesiredHouseCount() && ensure(HouseTypes.Num() > 0))
	{
		const int RandIdx = FMath::RandRange(0, HouseTypes.Num() - 1);
		if (SpawnNearby(HouseTypes[RandIdx]))
		{
			return;
		}
	}

	// TODO spawn windmills, sewers, etc.
	// and graveyard?
}

bool ACastle::SpawnNearby(const UBuildingType* Type)
{
	auto TeamState = GetTeamState();
	if (!ensure(TeamState))
	{
		return false;
	}

	// Going to iterate all buildings, moving out from the castle and attempting random spots around each.
	auto TeamBuildings = TeamState->GetBuildings();
	if (!ensure(!TeamBuildings.IsEmpty())) // Castle should be there
	{
		return false;
	}

	auto CastlePos = GetActorLocation();
	TeamBuildings.Sort([CastlePos](const ABuilding& Left, const ABuilding& Right)
	{
		// return if Left should come before Right
		return FVector::DistSquaredXY(CastlePos, Left.GetActorLocation())
			< FVector::DistSquaredXY(CastlePos, Right.GetActorLocation());
	});

	// Create placement helper
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.bNoFail = true;
	APlacementActor* PlacementHelper = GetWorld()->SpawnActor<APlacementActor>(Params);
	PlacementHelper->SetMesh(Type->BuildingPlaceShape);

	const auto PlacedExtent = Type->BuildingPlaceShape->GetBounds().BoxExtent;
	const double PlacedMaxDim = FMath::Max(PlacedExtent.X, PlacedExtent.Y);
	const FVector UpVector = FVector(0, 0, 1);
	bool bSuccess = false;
	for (auto Building : TeamBuildings)
	{
		// Try 3 times with random rotations and distances per building
		constexpr int AttemptsPerBuilding = 3;
		FVector AnchorOrigin;
		FVector AnchorExtent;
		Building->GetActorBounds(true, AnchorOrigin, AnchorExtent);
		const double AnchorMaxDim = FMath::Max(AnchorExtent.X, AnchorExtent.Y);
		AnchorOrigin.Z -= AnchorExtent.Z; // Move origin to the bottom instead of center
		
		for (int i = 0; i < AttemptsPerBuilding; i++)
		{
			constexpr double MinDist = 250;
			constexpr double MaxDist = 750;
			const double MinRadius = MinDist + (PlacedMaxDim + AnchorMaxDim);
			const double MaxRadius = MaxDist + (PlacedMaxDim + AnchorMaxDim);
			const FVector Offset = FVector(FMath::RandRange(MinRadius, MaxRadius), 0, 0).RotateAngleAxis(FMath::RandRange(0.0, 360.0), UpVector);
			const FVector WorldPos = AnchorOrigin + Offset;

			// Do cheaper placement actor check first
			PlacementHelper->SetActorLocation(WorldPos, false);
			if (PlacementHelper->HasOverlap())
			{
				continue;
			}

			if (AutoPlaceBuilding(Type, WorldPos))
			{
				bSuccess = true;
				break;
			}
		}

		if (bSuccess)
		{
			break;
		}
	}
	PlacementHelper->Destroy();

	return bSuccess;
}

bool ACastle::AutoPlaceBuilding(const UBuildingType* Type, FVector At)
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform Transform(FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f), At);
	auto Building = GetWorld()->SpawnActor<ABuilding>(Type->BuildingClass, Transform, Params);

	if (!Building)
	{
		return false;
	}
	
	Building->SetTeam(GetTeam());
	Building->ClearFoliageAround();
	return true;
}
