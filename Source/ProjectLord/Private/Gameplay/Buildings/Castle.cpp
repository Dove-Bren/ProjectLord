// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/Castle.h"

#include "Gameplay/GameTeamState.h"
#include "Gameplay/LordGameState.h"
#include "Gameplay/Player/PlacementActor.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/BuildingTypes.h"
#include "Gameplay/Buildings/SpawningBuilding.h"
#include "Gameplay/Units/HeroBase.h"
#include "Gameplay/Units/UnitTypes.h"

void ACastle::BeginPlay()
{
	Super::BeginPlay();

	NextSpawnTime = GetGameTimeSinceCreation() + .5f;
	bStartActivated = true;
	bCanAcceptTaxes = true;
}

void ACastle::ServerTick(float DeltaSeconds)
{
	Super::ServerTick(DeltaSeconds);

	float Time = GetGameTimeSinceCreation();
	if (Time > NextSpawnTime)
	{
		NextSpawnTime = Time + 10 + FMath::RandRange(5.0f, 20.0f);
		AttemptSpawnRound();
	}

	if (int Gold = Server_CollectBuildingGold())
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

int ACastle::GetSewerCount() const
{
	int Count = 0;
	auto TeamState = GetTeamState();
	if (ensure(TeamState))
	{
		// just here to make this visible
		ensure(!SewerTypes.IsEmpty());
		for (auto SewerType : SewerTypes)
		{
			Count += TeamState->GetTeamBuildingsOfType(SewerType).Num();
		}
	}

	return Count;
}

int ACastle::GetCemeteryCount() const
{
	int Count = 0;
	auto TeamState = GetTeamState();
	if (ensure(TeamState))
	{
		// just here to make this visible
		ensure(!CemeteryTypes.IsEmpty());
		for (auto CemeteryType : CemeteryTypes)
		{
			Count += TeamState->GetTeamBuildingsOfType(CemeteryType).Num();
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

int ACastle::GetDesiredSewerCount() const
{
	const int BuildingCount = GetBuildingCount();
	if (BuildingCount < 8)
	{
		return 0;
	}
	if (BuildingCount < 16)
	{
		return 1;
	}
	return 2;
}

int ACastle::GetDesiredCemeteryCount() const
{
	auto TeamState = GetTeamState();
	if (ensure(TeamState))
	{
		return TeamState->GetDeadHeroCount() > 0 ? 1 : 0;
	}
	//else
	return 0;
}

int ACastle::GetDesiredTaxCollectorCount() const
{
	// Idk why but this always seems to be 6
	return 6;
}

int ACastle::GetDesiredRoyalGuardCount() const
{
	// This is either always 6, or 3+level (4, 5, 6) in ref game
	// Just going to do 0, 1, 2
	return FMath::Max(0, GetBuildingLevel() - 1);//3 + GetBuildingLevel();
}

void ACastle::AttemptSpawnRound()
{

	if (GetHouseCount() < GetDesiredHouseCount() && ensure(HouseTypes.Num() > 0))
	{
		const int RandIdx = FMath::RandRange(0, HouseTypes.Num() - 1);
		if (SpawnNearby(HouseTypes[RandIdx], true))
		{
			return;
		}
	}

	if (GetSewerCount() < GetDesiredSewerCount() && ensure(SewerTypes.Num() > 0))
	{
		const int RandIdx = FMath::RandRange(0, SewerTypes.Num() - 1);
		if (SpawnNearby(SewerTypes[RandIdx], false))
		{
			return;
		}
	}

	if (GetCemeteryCount() < GetDesiredCemeteryCount() && ensure(CemeteryTypes.Num() > 0))
	{
		const int RandIdx = FMath::RandRange(0, CemeteryTypes.Num() - 1);
		if (SpawnNearby(CemeteryTypes[RandIdx], false))
		{
			return;
		}
	}

	// TODO spawn windmills
}

bool ACastle::SpawnNearby(const UBuildingType* Type, bool bFriendly)
{
	auto TeamState = GetTeamState();
	if (!ensure(TeamState))
	{
		return false;
	}

	// Going to iterate all buildings, moving out from the castle and attempting random spots around each.
	TArray<ABuilding*> TeamBuildings = TeamState->GetBuildings(); // COPY
	if (!ensure(!TeamBuildings.IsEmpty())) // Castle should be there
	{
		return false;
	}

	// Remove nullptrs (may not have replicated yet)
	TeamBuildings.RemoveAll([](const ABuilding* Building) {return !Building; });

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

			if (AutoPlaceBuilding(Type, WorldPos, bFriendly))
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

bool ACastle::AutoPlaceBuilding(const UBuildingType* Type, FVector At, bool bFriendly)
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FTransform Transform(FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f), At);
	auto Building = GetWorld()->SpawnActor<ABuilding>(Type->BuildingClass, Transform, Params);

	if (!Building)
	{
		return false;
	}
	
	Building->Server_SetTeam(GetTeam());
	Building->ClearFoliageAround();
	Building->PaintFoundation();

	if (auto SpawningBuilding = Cast<ASpawningBuilding>(Building))
	{
		// if bFriendly, make spawn team be our team.
		// Else, make it either monsters or neutral depending on if WE are monsters
		EGameTeam NewSpawnTeam = bFriendly ? GetTeam()
			: ((GetTeam() == EGameTeam::Monster) ? EGameTeam::Neutral : EGameTeam::Monster);
		SpawningBuilding->SetSpawnTeam(NewSpawnTeam);
	}

	return true;
}
